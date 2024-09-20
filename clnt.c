#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>

#define USR_NOTE "notes.txt" // 쪽지 정보
#define BUF_SIZE 100         // 메시지 버퍼의 크기
#define MAX_ATTEMPTS 3       // 최대 로그인 시도 횟수
#define NOTE_SIZE 100        // 쪽지 내용의 최대 크기

// 색상 코드 정의
#define RED "\033[31m"
#define RED_BOLD "\033[1;31m"
#define BLUE "\033[34m"
#define PURPLE "\033[47m"
#define GREY "\033[1;30m"
#define PURPLE_BOLD "\033[1;35m"
#define GREEN "\033[38;2;35;101;51m"
#define BOLD "\033[1;0m"
#define END "\033[0m"

/* 쪽지 저장 Note 구조체 정의 */
typedef struct
{
    int index;               // 쪽지 저장 번호
    char sender[BUF_SIZE];   // 보낸 사용자 이름
    char receiver[BUF_SIZE]; // 받는 사용자 이름
    char content[BUF_SIZE];  // 쪽지 내용
    int readYN;              // 읽음(0) or 안읽음(1)
    int deleteYN;            // 삭제(0) or 보관(1)
    char sendTime[BUF_SIZE]; // 전송 시간 문자열
} Note;

Note user_note_list[NOTE_SIZE]; // 쪽지 리스트(사용자)
int note_count = 0;             // 현재 저장된 쪽지의 수
size_t array_size;
char username[BUF_SIZE];
char password[BUF_SIZE];
// char roomname[BUF_SIZE];
int currentroom = 0;

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(const char *msg); // 에러 메시지를 출력하고 프로그램을 종료하는 함수
char message[BUF_SIZE];
void show_note_menu(int sock);
Note *handle_notes(Note *notes, size_t count, int sock);
void display_notes(Note *notes, size_t count);
int send_note(int sock, Note *note);
int get_next_index();
void createRoom(int sock);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_adr;
    int str_len;
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

                if (strncmp(message, "로그인 성공", 12) == 0)
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
            if (strncmp(message, "회원가입이 완료되었습니다.", 24) == 0)
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
        else if (strncmp(message, "/note\n", 5) == 0 || strcmp(message, "/NOTE\n") == 0)
        {
            show_note_menu(sock);
        }
        else if (strcmp(message, "/makeroom\n") == 0)
        {
            if (currentroom == 0)
            {
                write(sock, "/makeroom", strlen("/makeroom"));
            }
            else
            {
                printf("현재 채팅방에서는 채팅방을 개설할 수 없습니다.\n");
                break;
            }
        }
        else if (strcmp(message, "/room\n") == 0)
        {
            write(sock, "/room", strlen("/room"));
        }
        else if (strcmp(message, "/leaveroom\n") == 0)
        {
            write(sock, "/leaveroom", strlen("/leaveroom"));
        }
        else if (strcmp(message, "/leaveroom\n") == 0)
        {
            write(sock, "/leaveroom", strlen("/leaveroom"));
        }
        else
        {
            write(sock, message, strlen(message)); // 일반 메시지 전송
        }
    }
    return NULL;
}

void *recv_msg(void *arg) // read thread main
{
    int sock = *((int *)arg);
    char message[BUF_SIZE * 2]; // 버퍼 크기를 2배로 늘림
    int str_len;

    while (1)
    {
        str_len = read(sock, message, sizeof(message) - 1);
        if (str_len == -1)
        {
            perror("read() error");
            break;
        }

        if (str_len == 0)
        {
            printf("서버 연결이 종료되었습니다.\n");
            break;
        }

        message[str_len] = '\0'; // 수신된 메시지를 Null로 종료
        fputs(message, stdout);  // 메시지를 출력

        if (strncmp(message, "로그아웃 성공", 5) == 0)
        {
            printf("서버에서 로그아웃 허락 받았어!!\n");
            break;
        }
        else if (strncmp(message, "note_send", 10) == 0)
        {
        }
        else if (strncmp(message, "note_check", 10) == 0)
        {
            // note_check를 제외한 메시지 부분 읽기
            size_t size = strtoul(message + 10, NULL, 10);

            // 구조체 배열 수신
            ssize_t received = 0;
            Note *arr = malloc(size); // 동적 할당
            if (arr == NULL)
            {
                perror("malloc failed");
                break;
            }

            while (received < size)
            {
                ssize_t bytes = recv(sock, (char *)arr + received, size - received, 0);
                if (bytes < 0)
                {
                    perror("recv failed");
                    free(arr);
                    break;
                }
                received += bytes;
            }

            if (received == 0)
            {
                printf("수신된 쪽지가 없습니다.\n");
                free(arr);
                continue;
            }
            else
            {
                Note *notes = handle_notes(arr, size / sizeof(Note), sock);

                // 서버에 업데이트 알리기
                // Read 처리
                char message[1024] = {0};
                snprintf(message, sizeof(message), "/update_note_read");

                // Read 인덱스 추가
                for (size_t i = 0; i < size / sizeof(Note); ++i)
                {
                    if (i > 0)
                        strncat(message, ",", sizeof(message) - strlen(message) - 1); // ',' 추가
                    char buffer[32];
                    snprintf(buffer, sizeof(buffer), "%d", notes[i].index);
                    strncat(message, buffer, sizeof(message) - strlen(message) - 1);
                }

                // Delete 처리
                strncat(message, "delete_", sizeof(message) - strlen(message) - 1); // delete_ 추가
                for (size_t i = 0; i < size / sizeof(Note); ++i)
                {
                    if (notes[i].deleteYN == 1)
                    {
                        if (strlen(message) > strlen("/update_note_read"))
                            strncat(message, ",", sizeof(message) - strlen(message) - 1); // ',' 추가
                        char buffer[32];
                        snprintf(buffer, sizeof(buffer), "%d", notes[i].index);
                        strncat(message, buffer, sizeof(message) - strlen(message) - 1);
                    }
                }

                // 메시지 전송
                write(sock, message, strlen(message));
                printf("전송된 메시지: %s\n", message);
            }

            free(arr);
        }
        else if (strncmp(message, "/makeroom", 9) == 0)
        {
            /* '/makeroom' 제거 및 숫자 변환 */
            const char *command = "/makeroom";
            size_t commandLength = strlen(command);

            // message가 '/makeroom'으로 시작하는지 확인
            if (strncmp(message, command, commandLength) == 0)
            {
                char *numStr = (char *)message + commandLength; // 명령어 뒤의 숫자 부분
        
                if ( atoi(numStr) > 10)
                {
                    printf("최대 방 수를 초과했습니다. 방을 더 이상 만들 수 없습니다.");
                    break;
                }
                else
                {
                    currentroom = atoi(numStr); // 숫자로 변환
                    printf("%s채팅방%s에 입장했습니다.%s\n", RED_BOLD, numStr, END);
                }
            }
        }
        else if (strncmp(message, "/leaveroom", 10) == 0)
        {
            /* '/leaveroom' 제거 및 숫자 변환 */
            const char *command = "/leaveroom";
            size_t commandLength = strlen(command);

            // message가 '/leaveroom'으로 시작하는지 확인
            if (strncmp(message, command, commandLength) == 0)
            {
                char *numStr = (char *)message + commandLength; // 명령어 뒤의 숫자 부분
                
                if (atoi(numStr) == 0)
                {
                    currentroom = atoi(numStr); // 숫자로 변환
                    printf("%s전체 채팅방에 입장했습니다.%s\n", RED_BOLD, END);
                }
                else
                {
                    printf("오류\n");
                    break;
                }
            }
        }
        else if (strncmp(message, "/inviteroom", 11) == 0)
        {
            
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

void initialize_note_list(Note note_list[], int size)
{
    for (int i = 0; i < size; i++)
    {
        note_list[i].index = 0;
        strcpy(note_list[i].sender, "");
        strcpy(note_list[i].receiver, "");
        strcpy(note_list[i].content, "");
        note_list[i].readYN = 0;
        note_list[i].deleteYN = 0;
        strcpy(note_list[i].sendTime, "");
    }
}

void show_note_menu(int sock)
{
    char input[BUF_SIZE];

    // 메뉴 출력
    printf("1. 쪽지 보내기\n");
    printf("2. 쪽지 확인하기\n");
    printf("3. 취소\n");
    printf("선택하세요: ");

    // 사용자 입력 받기
    if (fgets(input, BUF_SIZE, stdin) == NULL)
    {
        fprintf(stderr, "입력 오류\n"); // 입력 오류 처리
        return;
    }

    // 쪽지 보내기 선택
    if (strcmp(input, "1\n") == 0)
    {
        printf("쪽지 보내기 선택\n");
        Note sendNote;
        time_t now;
        struct tm *tm_info;

        // 인덱스 설정 (서버에서 관리)
        sendNote.index = get_next_index();

        // 보낸 사람 설정 (사용자 이름 사용)
        if (username == NULL)
        {
            fprintf(stderr, "사용자 이름이 설정되지 않았습니다.\n");
            return; // 사용자 이름이 없으면 종료
        }
        strcpy(sendNote.sender, username);

        // 받는 사람 입력
        printf("받는 사람 : ");
        if (fgets(sendNote.receiver, BUF_SIZE, stdin) == NULL)
        {
            fprintf(stderr, "입력 오류\n"); // 입력 오류 처리
            return;
        }
        sendNote.receiver[strcspn(sendNote.receiver, "\n")] = '\0'; // 개행 문자 제거

        // 내용 입력
        printf("내용 : ");
        if (fgets(sendNote.content, BUF_SIZE, stdin) == NULL)
        {
            fprintf(stderr, "입력 오류\n"); // 입력 오류 처리
            return;
        }
        sendNote.content[strcspn(sendNote.content, "\n")] = '\0'; // 개행 문자 제거

        // 읽음 여부 및 삭제 여부 기본값 설정
        sendNote.readYN = 0;
        sendNote.deleteYN = 0;

        // 현재 시간 가져오기
        time(&now);
        tm_info = localtime(&now);
        strftime(sendNote.sendTime, sizeof(sendNote.sendTime), "%Y-%m-%d %H:%M:%S", tm_info); // 시간 형식 설정

        // 서버에 쪽지 전송
        if (send_note(sock, &sendNote) < 0)
        {
            fprintf(stderr, "쪽지 전송 오류\n"); // 전송 오류 처리
        }
    }
    else if (strcmp(input, "2\n") == 0)
    {
        printf("쪽지 확인하기 선택\n");
        // 쪽지 확인하기
        write(sock, "/note_check\n", strlen("/note_check\n"));

        printf("쪽지 확인 요청을 서버에 보냈습니다.\n");
    }
    else if (strcmp(input, "3\n") == 0)
    {
        // 취소
        printf("취소되었습니다.\n");
    }
}

Note *handle_notes(Note *notes, size_t count, int sock)
{
    // sleep(2); // 잠시 대기
    int check = 1000;

    while (check != 0)
    {
        if (count == 0)
        {
            puts("수신된 쪽지가 없습니다. 0을 입력하면 종료됩니다.\n");
        }

        system("clear");
        puts("\n* * * * 받은 쪽지함 * * * *\n");

        // Display notes (function implementation not provided)
        display_notes(notes, count);

        puts("쪽지번호 : 삭제 , 0 : 닫기");
        printf("\n입력: ");
        fflush(stdout);
        // scanf("%d", &check);
        // while (getchar() != '\n')
        //     ; // Clear input buffer

        if (check > 0 && check <= (int)count)
        {
            notes[check - 1].deleteYN = 1; // 해당 쪽지 삭제
        }
        else if (check == 0)
        {
            // 종료 처리
            printf("쪽지함을 닫습니다.\n");
            break;
        }
        else
        {
            printf("잘못된 번호입니다. 다시 입력하세요.\n");
        }
    }
    printf("--쪽지함 확인 종료--\n");

    return notes;
}

void display_notes(Note *notes, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        // 삭제되지 않은 쪽지만 출력
        if (notes[i].deleteYN != 1)
        {
            printf("\n");
            if (notes[i].readYN == 1)
            {
                // 새로운 쪽지일 경우
                printf("%sNEW%s\n", RED_BOLD, END);
                notes[i].readYN = 0; // 읽음 상태를 업데이트
            }
            else
            {
                printf("%-3zu\n", i + 1);
            }
            // 쪽지 정보 출력
            printf("%s[To. %s]%s\n", BOLD, notes[i].sender, END);
            printf("%s%s%s\n", GREY, notes[i].content, END);
            printf("%s\n", notes[i].sendTime);
            printf("==============================================\n");
        }
    }
}

int send_note(int sock, Note *note)
{
    // 메시지를 생성
    int len = snprintf(message, sizeof(message),
                       "/note_send %d %s %s %s %d %d %s\n",
                       note->index,
                       note->sender,
                       note->receiver,
                       note->content,
                       note->readYN,
                       note->deleteYN,
                       note->sendTime);

    // snprintf 오류 처리: 반환값이 0 이하일 경우 에러 발생
    if (len < 0)
    {
        perror("snprintf() error"); // 에러 메시지 출력
        return -1;                  // 에러 반환
    }

    // 버퍼 크기 초과 경고: 생성된 문자열 길이가 버퍼 크기를 초과할 경우
    if (len >= sizeof(message))
    {
        fprintf(stderr, "Warning: 메시지가 잘립니다.\n"); // 경고 메시지 출력
        return -1;                                        // 에러 반환
    }

    // 소켓에 메시지를 쓰기
    if (write(sock, message, len) < 0)
    {
        perror("write() error"); // 쓰기 오류 발생 시 에러 메시지 출력
        return -1;               // 에러 반환
    }
    else
    {
        printf("Note sent to server: %s\n", message); // 전송된 메시지 출력
        return 0;                                     // 성공적으로 전송됨
    }
}

int get_next_index()
{
    FILE *file = fopen(USR_NOTE, "r");
    if (!file)
    {
        perror("fopen() error");
        return 1;
    }

    int last_index = 0;
    char line[BUF_SIZE * 2];
    while (fgets(line, sizeof(line), file))
    {
        int index;
        if (sscanf(line, "%d", &index) == 1)
        {
            last_index = index;
        }
    }
    fclose(file);
    return last_index + 1;
}

void createRoom(int sock)
{
}