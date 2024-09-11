#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

// 색상 코드 정의
#define COLOR_RESET "\033[0m"
#define COLOR_RED_BOLD "\033[1;31m"
#define COLOR_BLUE "\033[34m"

int sock; // 서버와의 소켓 연결
int is_logged_in = 0; // 로그인 상태 플래그
char username[MAX_USERNAME_LENGTH + 1]; // 사용자 이름 저장

// 서버에서 메시지를 읽어 출력하는 스레드 함수
void *receive_messages(void *arg) {
    char buf[BUF_SIZE]; // 수신 버퍼
    int str_len;

    while (1) {
        memset(buf, 0, sizeof(buf)); // 버퍼 초기화
        str_len = read(sock, buf, sizeof(buf) - 1); // 서버로부터 메시지 읽기
        if (str_len <= 0) { // 읽기 실패 또는 연결 종료
            perror("read() error or connection closed");
            close(sock); // 소켓 닫기
            exit(EXIT_FAILURE); // 프로그램 종료
        }

        buf[str_len] = '\0'; // 수신된 문자열의 널 종료

        // 공지 메시지의 경우 색상 처리
        if (strstr(buf, "공지") != NULL) {
            printf("%s[공지] : %s%s", COLOR_RED_BOLD, buf, COLOR_RESET);
        } else {
            // 자신이 보낸 메시지인 경우 파란색으로 출력
            if (strncmp(buf, username, strlen(username)) == 0) {
                printf("%s[나]: %s%s", COLOR_BLUE, buf, COLOR_RESET);
            } else {
                // 다른 메시지는 기본 색상으로 출력
                printf("%s", buf);
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr; // 서버 주소 구조체
    char buf[BUF_SIZE]; // 입력 버퍼
    int choice; // 사용자 입력
    pthread_t recv_thread; // 수신 스레드

    // 커맨드라인 인자 검증
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); // IP 주소
    server_addr.sin_port = htons(atoi(argv[2])); // 포트 번호

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() error");
        close(sock); // 소켓 닫기
        exit(EXIT_FAILURE);
    }

    printf("서버에 연결되었습니다. 채팅을 시작합니다.\n");

    // 서버로부터 메시지를 비동기적으로 수신하기 위한 스레드 생성
    if (pthread_create(&recv_thread, NULL, receive_messages, NULL) != 0) {
        perror("pthread_create() error");
        close(sock); // 소켓 닫기
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (!is_logged_in) { // 로그인 상태가 아닌 경우
            // 로그인 또는 회원 가입 메뉴 출력
            printf("채팅 서버에 오신 것을 환영합니다. 로그인하거나 등록해주세요.\n");
            printf("원하는 작업을 선택하세요 (1: 로그인, 2: 회원 가입, 0: 종료): ");
            if (scanf("%d", &choice) != 1) {
                printf("잘못된 입력입니다.\n");
                while (getchar() != '\n'); // 입력 버퍼 비우기
                continue;
            }
            while (getchar() != '\n'); // 줄 바꿈 문자 제거

            if (choice == 1 || choice == 2) { // 로그인 또는 회원 가입 선택
                printf("사용자 이름과 비밀번호를 입력하세요: ");
                if (scanf("%100s %100s", username, buf) != 2) {
                    printf("잘못된 입력입니다.\n");
                    while (getchar() != '\n'); // 입력 버퍼 비우기
                    continue;
                }
                while (getchar() != '\n'); // 줄 바꿈 문자 제거

                // 메시지 생성
                char message[BUF_SIZE];
                int message_len = snprintf(message, sizeof(message), "%d %s %s", choice, username, buf);

                // 메시지가 버퍼를 초과할 경우 잘라내기
                if (message_len >= sizeof(message)) {
                    fprintf(stderr, "메시지가 너무 깁니다. 잘라내겠습니다.\n");
                    message[sizeof(message) - 1] = '\0'; // 메시지 잘라내기
                }

                // 서버에 메시지 전송
                if (write(sock, message, strlen(message)) == -1) {
                    perror("write() error");
                    close(sock); // 소켓 닫기
                    exit(EXIT_FAILURE);
                }

                // 로그인 또는 회원 가입 결과 확인
                memset(buf, 0, sizeof(buf)); // 입력 버퍼 초기화
                if (read(sock, buf, sizeof(buf) - 1) <= 0) {
                    perror("read() error or connection closed");
                    close(sock); // 소켓 닫기
                    exit(EXIT_FAILURE);
                }

                // 로그인 또는 회원가입 결과 처리
                if (choice == 2 && strncmp(buf, "회원 가입 실패", 13) == 0) {
                    printf("회원 가입 실패: 아이디가 이미 존재합니다.\n");
                    // 회원 가입 실패 시 다시 메뉴로 돌아가서 입력 받기
                    continue;
                }

                if (choice == 2) { // 회원 가입 후
                    printf("회원 가입이 완료되었습니다. 로그인 해주세요.\n");
                    is_logged_in = 0; // 로그인 상태를 초기화
                    continue; // 로그인 화면으로 돌아가기
                }

                if (choice == 1 && strncmp(buf, "로그인 성공", 12) == 0) {
                    is_logged_in = 1; // 로그인 상태로 변경
                    printf("로그인 성공! 채팅을 시작합니다.\n");
                } else if (choice == 1) {
                    printf("로그인 실패: 아이디 또는 비밀번호가 잘못되었습니다.\n");
                }
            } else if (choice == 0) { // 종료 선택
                if (write(sock, "0", 1) == -1) {
                    perror("write() error");
                    close(sock); // 소켓 닫기
                    exit(EXIT_FAILURE);
                }
                close(sock); // 소켓 닫기
                exit(EXIT_SUCCESS); // 프로그램 종료
            } else {
                printf("잘못된 옵션입니다. 다시 시도하세요.\n");
            }
        } else {
            // 로그인 상태에서 클라이언트가 연결된 경우 채팅 기능 수행
            while (1) {
                printf("메시지를 입력하세요: ");
                if (fgets(buf, sizeof(buf), stdin) == NULL) {
                    perror("fgets() error");
                    break;
                }

                // 문자열 길이 계산
                size_t buf_len = strlen(buf);
                
                // 개행 문자 제거
                if (buf[buf_len - 1] == '\n') {
                    buf[buf_len - 1] = '\0';
                    buf_len--;
                }

                // 메시지 길이를 체크하고 포맷
                char message[BUF_SIZE];
                int message_len = snprintf(message, sizeof(message), "%s: %s", username, buf);

                // 메시지가 버퍼를 초과할 경우 잘라내기
                if (message_len >= sizeof(message)) {
                    fprintf(stderr, "메시지가 너무 깁니다. 잘라내겠습니다.\n");
                    message[sizeof(message) - 1] = '\0'; // 메시지 잘라내기
                }

                // 서버에 메시지 전송
                if (write(sock, message, strlen(message)) == -1) {
                    perror("write() error");
                    break;
                }
            }
            // 채팅 종료 후 클라이언트 종료
            close(sock); // 소켓 닫기
            exit(EXIT_SUCCESS); // 프로그램 종료
        }
    }

    close(sock); // 소켓 닫기 (루프가 종료될 때)
    return 0;
}

/* 
클라이언트:

서버로부터 회원 가입 실패 메시지를 받을 경우, 해당 메시지를 출력하고 다시 회원 가입 입력을 받습니다.
로그인 성공 또는 실패 시 적절한 메시지를 출력합니다.
*/

/*

헤더 파일 및 매크로 정의

BUF_SIZE, MAX_USERNAME_LENGTH, MAX_PASSWORD_LENGTH는 버퍼 크기 및 최대 사용자 이름과 비밀번호 길이를 정의합니다.
색상 코드는 터미널에서 출력 색상을 지정하는 ANSI escape 코드입니다.
전역 변수

sock: 서버와의 소켓 연결을 저장합니다.
is_logged_in: 로그인 상태를 나타내는 플래그입니다.
username: 사용자 이름을 저장합니다.
receive_messages 함수

서버로부터 메시지를 수신하고 출력하는 스레드 함수입니다.
공지 메시지는 빨간색으로 출력하고, 자신이 보낸 메시지는 파란색으로 출력합니다.
다른 메시지는 기본 색상으로 출력합니다.
main 함수

서버와의 연결을 설정하고, 사용자 입력을 받아 서버와 통신합니다.
pthread_create를 사용하여 receive_messages 함수를 비동기적으로 실행합니다.
사용자가 로그인 또는 회원 가입을 선택하면 서버에 메시지를 전송합니다.
로그인 성공 후 채팅 기능으로 전환하며, 채팅 종료 후 클라이언트를 종료합니다.

 */