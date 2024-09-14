#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>

#define BUF_SIZE 100   // 메시지 버퍼의 크기
#define MAX_ATTEMPTS 3 // 최대 로그인 시도 횟수

// 색상 코드 정의
#define RESET "\033[0m"
#define RED_BOLD "\033[1;31m"
#define BLUE "\033[34m"
#define PURPLE "\033[47m"    

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(const char *msg); // 에러 메시지를 출력하고 프로그램을 종료하는 함수
char message[BUF_SIZE];

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_adr;
    int str_len;
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    int choice;
    int attempt_count;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;
    fd_set read_fds;
    int fd_max;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    /* socket() : 서버 소켓을 생성하고, */
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    /* 서버 주소 설정 */
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    /* connect() : 서버에 연결 */
    if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    fd_max = sock; // 최대 파일 디스크립터 초기화
    int login = 1;

    /* 서버로부터의 환영 메시지를 수신 */
    str_len = read(sock, message, sizeof(message) - 1);
    if (str_len == -1)
        error_handling("read() error");

    message[str_len] = 0;
    printf("%s", message);

    /* 메뉴 선택 루프 */
    while (login == 1)
    {
        printf("원하는 작업을 선택하세요 (1: 로그인, 2: 회원 가입 0: 종료): ");
        if (scanf("%d", &choice) != 1)
        {
            fprintf(stderr, "Invalid input\n");
            while (getchar() != '\n')
                ; // 입력 버퍼 비우기
            continue;
        }
        getchar(); // 입력 버퍼에서 개행 문자를 제거

        if (choice == 0)
        {
            printf("연결을 종료합니다.\n");
            close(sock);
            return 0;
        }
        else if (choice == 1) // 로그인 선택
        {
            attempt_count = 0; // 로그인 시도 횟수 초기화
            while (attempt_count < MAX_ATTEMPTS)
            {
                printf("사용자 이름과 비밀번호를 입력하세요: ");
                if (scanf("%99s %99s", username, password) != 2)
                {
                    fprintf(stderr, "Invalid input\n");
                    while (getchar() != '\n')
                        ; // 입력 버퍼 비우기
                    continue;
                }
                getchar(); // 입력 버퍼에서 개행 문자를 제거

                // `snprintf()`로 문자열을 안전하게 포맷
                int len = snprintf(message, sizeof(message), "login %s %s", username, password);
                if (len >= sizeof(message))
                {
                    fprintf(stderr, "Error: Message too long\n");
                    continue;
                }

                if (write(sock, message, strlen(message)) == -1)
                    error_handling("write() error");

                // 서버로부터의 응답 메시지 수신
                str_len = read(sock, message, sizeof(message) - 1);
                if (str_len == -1)
                    error_handling("read() error");

                message[str_len] = 0;
                printf("%s\n", message);

                if (strncmp(message, "로그인 성공", 6) == 0)
                {
                    // 로그인 성공 시 메시지 전송 루프
                    login = 0;
                    break; // 로그인 후 메시지 전송 루프 종료
                }
                else
                {
                    attempt_count++;
                    if (attempt_count < MAX_ATTEMPTS)
                    {
                        printf("로그인 실패. 남은 시도 횟수: %d\n", MAX_ATTEMPTS - attempt_count);
                    }
                    else
                    {
                        printf("로그인 시도 횟수를 초과했습니다. 처음 화면으로 돌아갑니다.\n");
                        break; // 로그인 시도 횟수를 초과하면 루프 종료
                    }
                }
            }
            continue; // 메뉴 화면으로 돌아가기
        }
        else if (choice == 2) // 회원 가입 선택
        {
            printf("사용자 이름과 비밀번호를 입력하세요: ");
            if (scanf("%99s %99s", username, password) != 2)
            {
                fprintf(stderr, "Invalid input\n");
                while (getchar() != '\n')
                    ; // 입력 버퍼 비우기
                continue;
            }
            getchar(); // 입력 버퍼에서 개행 문자를 제거

            // `snprintf()`로 문자열을 안전하게 포맷
            int len = snprintf(message, sizeof(message), "register %s %s", username, password);
            if (len >= sizeof(message))
            {
                fprintf(stderr, "Error: Message too long\n");
                continue;
            }

            if (write(sock, message, strlen(message)) == -1)
                error_handling("write() error");

            // 서버로부터의 응답 메시지 수신
            str_len = read(sock, message, sizeof(message) - 1);
            if (str_len == -1)
                error_handling("read() error");

            message[str_len] = 0;
            printf("%s\n", message);

            // 회원가입 후 로그인 화면으로 돌아가기
            if (strncmp(message, "회원가입이 완료되었습니다.", 14) == 0)
            {
                printf("회원가입이 완료되었습니다. 로그인할 수 있습니다.\n");
                continue; // 메뉴 화면으로 돌아가기
            }
        }
        else
        {
            printf("잘못된 선택입니다. 다시 시도하세요.\n");
        }
    }

    pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);

    close(sock);
    return 0;
}
void *send_msg(void *arg) // send thread main
{
    int sock = *((int *)arg);
    char message[BUF_SIZE];
    while (1)
    {
        fgets(message, BUF_SIZE, stdin);
        if (strcmp(message, "/exit\n") == 0 || strcmp(message, "/EXIT\n") == 0)
        {
            // 서버에 로그아웃 요청 전송
            write(sock, "/exit", strlen("/exit"));
        }
        else
        {
            // 일반 메시지 전송
            write(sock, message, strlen(message));
        }
    }
    return NULL;
}

void *recv_msg(void *arg) // read thread main
{
    int sock = *((int *)arg);
    char message[BUF_SIZE * 2];  // 버퍼 크기를 2배로 늘림
    int str_len;

    while (1)
    {
        str_len = read(sock, message, sizeof(message) - 1);
        if (str_len == -1)
        {
            perror("read() error");
            return (void *)-1;
        }

        if (str_len == 0)
        {
            printf("서버 연결이 종료되었습니다.\n");
            return NULL;
        }

        message[str_len] = '\0';  // 수신된 메시지를 Null로 종료
        fputs(message, stdout);   // 메시지를 출력

        if (strncmp(message, "로그아웃 성공", 7) == 0)
        {
            // printf("서버에서 로그아웃 허락 받았어!!\n");
            break;
        }
    }

    // 클라이언트 소켓 닫기
    close(sock);
    exit(0);
    return NULL;
}

void error_handling(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}