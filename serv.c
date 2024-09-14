#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

#define BUF_SIZE 100         // 메시지와 파일 버퍼의 크기
#define NOTE_SIZE 100        // 쪽지 내용의 최대 크기
#define MAX_CLNT 256         // 서버가 동시에 처리할 수 있는 최대 클라이언트 수
#define USR_FILE "users.txt" // 사용자 정보를 저장할 파일명
#define USR_NOTE "notes.txt" // 쪽지 정보

#define RED "\033[31m"
#define RED_BOLD "\033[1;31m"
#define BLUE "\033[34m" -
#define PURPLE "\033[47m"
#define PINK_BOLD "\033[1;35m"
#define GREEN "\033[38;2;35;101;51m"
#define BOLD "\033[1;0m"
#define END "\033[0m"

/* 현재 클라이언트 소켓번호, 이름 저장 구조체 */
typedef struct
{
    int sock;                // 클라이언트 소켓
    char username[BUF_SIZE]; // 사용자 이름
    int loggedIn;            // 로그인 상태 (1: 로그인됨, 0: 로그인 안됨)
} Client;

/* 쪽지 저장 구조체 */
typedef struct
{
    int index;               // 쪽지 저장 번호
    char sender[BUF_SIZE];   // 보낸 사용자 이름
    char receiver[BUF_SIZE]; // 받는 사용자 이름
    char content[NOTE_SIZE]; // 쪽지 내용
    int readYN;              // 읽음(0) or 안읽음(1)
    int deleteYN;            // 삭제(0) or 보관(1)
    char *sendTime;          // 전송 시간 문자열 포인터
} Note;

int clnt_cnt = 0;            // 현재 연결된 클라이언트의 수
Client clnt_socks[MAX_CLNT]; // 연결된 클라이언트의 소켓 파일 디스크립터
pthread_mutex_t mutx;        // 클라이언트 소켓 배열에 대한 동시 접근을 동기화하기 위한 뮤텍스
Note note_list[NOTE_SIZE];   // 노트 리스트

void *server_input(void *arg);
void *handle_clnt(void *arg);                                   // 클라이언트와의 연결을 처리하는 쓰레드의 메인 함수
void error_handling(const char *msg);                           // 에러 메시지를 출력하고 프로그램을 종료하는 함수
void send_msg(const char *msg, int len, const char *sender);    // 모든 클라이언트에게 메시지를 전송하는 함수
int is_user_valid(const char *username, const char *password);  // 사용자 인증을 수행하는 함수
void register_user(const char *username, const char *password); // 새 사용자를 등록하는 함수
int is_user_exists(const char *username);                       // 사용자가 이미 존재하는지 확인하는 함수
int is_user_logged_in(const char *username);                    // 사용자가 이미 로그인되어 있는지 확인하는 함수
void remove_client(int clnt_sock);                              // 클라이언트 소켓 제거

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id, input_thread;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);

    /* socket() : 서버 소켓을 생성하고, */
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    /* 포트 번호를 인수로 받아서 서버를 시작 */
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    /* bind() : 주소를 바인딩한 뒤, */
    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    /* listen() : 클라이언트의 연결 요청을 대기 */
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    /* 서버 입력을 처리할 쓰레드 생성 */
    pthread_create(&input_thread, NULL, server_input, NULL);
    pthread_detach(input_thread); // 서버 입력 쓰레드가 종료될 때 자원을 자동으로 반환

    /* accept() : 클라이언트가 연결되면 새로운 쓰레드를 생성하여 handle_clnt 함수를 실행 */
    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1)
        {
            perror("accept() failed");
            continue;
        }
        /* 클라이언트 연결 */
        pthread_mutex_lock(&mutx);
        int idx = clnt_cnt++; // 현재 클라이언트의 인덱스를 저장하고 증가시킴
        clnt_socks[idx].sock = clnt_sock;
        clnt_socks[idx].username[0] = '\0'; // 초기화
        clnt_socks[idx].loggedIn = 0;       // 로그인 상태 초기화

        pthread_mutex_unlock(&mutx);

        /* 새로운 쓰레드 생성하여 handle_clnt 함수 실행 */
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_socks[idx].sock);
        pthread_detach(t_id); // pthread_detach를 호출하여 쓰레드가 종료될 때 자원을 자동으로 반환
        printf("Connected client IP: %s\n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

/* 클라이언트 연결 이후 동작 함수 */
void *handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    int idx = -1;
    int is_authenticated = 0;

    const char *welcome_msg = "스몰톡에서는 스몰톡만 즐겨주시기 바랍니다. E는 돌아가주세요.\n";
    if (write(clnt_sock, welcome_msg, strlen(welcome_msg)) == -1)
    {
        perror("write() error");
        close(clnt_sock);
        return NULL;
    }

    /* 클라이언트 소켓 인덱스 찾기 */
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (clnt_socks[i].sock == clnt_sock)
        {
            idx = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutx);

    if (idx == -1)
    {
        perror("Client index not found");
        close(clnt_sock);
        return NULL;
    }

    // 인증되지 않은 클라이언트 상태 처리
    while (!is_authenticated && (str_len = read(clnt_sock, msg, sizeof(msg) - 1)) != 0)
    {
        if (str_len == -1)
        {
            perror("read() error");
            close(clnt_sock);
            return NULL;
        }

        msg[str_len] = '\0'; // Null-terminate the received message

        if (strncmp(msg, "register", 8) == 0)
        {
            sscanf(msg + 9, "%s %s", username, password);
            if (is_user_exists(username))
            {
                const char *err_msg = "이미 존재하는 사용자입니다.\n";
                if (write(clnt_sock, err_msg, strlen(err_msg)) == -1)
                {
                    perror("write() error");
                    close(clnt_sock);
                    return NULL;
                }
            }
            else
            {
                register_user(username, password);
                const char *success_msg = "회원가입이 완료되었습니다. 로그인할 수 있습니다.\n";
                if (write(clnt_sock, success_msg, strlen(success_msg)) == -1)
                {
                    perror("write() error");
                    close(clnt_sock);
                    return NULL;
                }
            }
        }
        else if (strncmp(msg, "login", 5) == 0)
        {
            sscanf(msg, "login %s %s", username, password);
            if (is_user_valid(username, password))
            {
                if (is_user_logged_in(username))
                {
                    const char *login_fail_msg = "이미 로그인된 사용자입니다.\n";
                    if (write(clnt_sock, login_fail_msg, strlen(login_fail_msg)) == -1)
                    {
                        perror("write() error");
                        close(clnt_sock);
                        return NULL;
                    }
                }
                else
                {
                    pthread_mutex_lock(&mutx);
                    strcpy(clnt_socks[idx].username, username);
                    clnt_socks[idx].loggedIn = 1; // 로그인 상태 설정
                    pthread_mutex_unlock(&mutx);

                    const char *login_success_msg = "로그인 성공\n";
                    if (write(clnt_sock, login_success_msg, strlen(login_success_msg)) == -1)
                    {
                        perror("write() error");
                        close(clnt_sock);
                        return NULL;
                    }

                    /* 로그인 알림 전송 */
                    char login_msg[BUF_SIZE + 50];
                    snprintf(login_msg, sizeof(login_msg), "%s%s님이 로그인 했습니다. %s\n", PINK_BOLD, clnt_socks[idx].username, END);
                    send_msg(login_msg, strlen(login_msg), clnt_socks[idx].username);
                    is_authenticated = 1;
                }
            }
            else
            {
                const char *login_fail_msg = "로그인 실패\n";
                if (write(clnt_sock, login_fail_msg, strlen(login_fail_msg)) == -1)
                {
                    perror("write() error");
                    close(clnt_sock);
                    return NULL;
                }
            }
        }
        else
        {
            const char *auth_prompt_msg = "로그인 또는 회원가입을 해주세요.\n";
            if (write(clnt_sock, auth_prompt_msg, strlen(auth_prompt_msg)) == -1)
            {
                perror("write() error");
                close(clnt_sock);
                return NULL;
            }
        }
    }

    // 인증된 클라이언트 처리
    while ((str_len = read(clnt_sock, msg, sizeof(msg) - 1)) != 0)
    {
        if (str_len == -1)
        {
            perror("read() error");
            close(clnt_sock);
            return NULL;
        }

        msg[str_len] = '\0'; // Null-terminate the received message

        if (strncmp(msg, "/user", 5) == 0)
        {
            pthread_mutex_lock(&mutx);
            char user_list[BUF_SIZE * MAX_CLNT];
            strcpy(user_list, "현재 접속중인 클라이언트:\n");
            for (int i = 0; i < clnt_cnt; i++)
            {
                strcat(user_list, clnt_socks[i].username);
                strcat(user_list, "\n");
            }
            pthread_mutex_unlock(&mutx);
            write(clnt_sock, user_list, strlen(user_list));
        }
        else if (strncmp(msg, "/exit", 5) == 0)
        {
            // 클라이언트의 로그아웃 상태로 변경
            clnt_socks[idx].loggedIn = 0;

            // 로그아웃 메시지 생성
            char exit_msg[BUF_SIZE + 50];
            snprintf(exit_msg, sizeof(exit_msg), "%s%s님이 로그아웃 했습니다. %s\n", PINK_BOLD, clnt_socks[idx].username, END);
            printf("로그아웃 메시지 생성: %s\n", exit_msg);  // 로그아웃 메시지 확인

            // 다른 클라이언트들에게 로그아웃 메시지 전송
            send_msg(exit_msg, strlen(exit_msg), clnt_socks[idx].username); // 모든 클라이언트에게 전송

            // 성공 허락하기
            const char *logout_success_msg = "로그아웃 성공\n";
            if (write(clnt_socks[idx].sock, logout_success_msg, strlen(logout_success_msg)) == -1)
            {
                perror("write() error");
            }

            // 소켓 제거 및 종료 처리
            remove_client(clnt_sock);

            return NULL;
        }
        else if (strncmp(msg, "/w", 2) == 0)
        {
            char receiver[BUF_SIZE];
            char message[BUF_SIZE];

            sscanf(msg, "/w %s %[^\n]", receiver, message);

            int toSocket = -1;
            for (int i = 0; i < clnt_cnt; i++)
            {
                if (strcmp(clnt_socks[i].username, receiver) == 0)
                {
                    toSocket = i;
                    break;
                }
            }

            if (toSocket != -1)
            {
                char r_message[BUF_SIZE * 2]; // 버퍼 크기를 증가시킴
                int written_len = snprintf(r_message, sizeof(r_message), "%s%s %s> %s%s\n", GREEN, "귓속말", clnt_socks[idx].username, message, END);

                // 버퍼 초과 확인
                if (written_len >= sizeof(r_message))
                {
                    const char *error_msg = "메시지가 너무 깁니다.\n";
                    write(clnt_sock, error_msg, strlen(error_msg));
                }
                else
                {
                    write(clnt_socks[toSocket].sock, r_message, strlen(r_message));
                }
            }
            else
            {
                const char *error_msg = "해당 사용자가 없습니다.\n";
                write(clnt_sock, error_msg, strlen(error_msg));
            }
        }
        else
        {
            char full_msg[BUF_SIZE];
            int full_len = snprintf(full_msg, sizeof(full_msg), "%s: %s", clnt_socks[idx].username, msg);
            send_msg(full_msg, full_len, clnt_socks[idx].username);
        }
    }

    remove_client(clnt_sock);
    return NULL;
}

void remove_client(int clnt_sock)
{
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (clnt_socks[i].sock == clnt_sock)
        {
            for (int j = i; j < clnt_cnt - 1; j++)
            {
                clnt_socks[j] = clnt_socks[j + 1];
            }
            clnt_cnt--;
            break;
        }
    }
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
}

void send_msg(const char *msg, int len, const char *sender)
{
    pthread_mutex_lock(&mutx);

    // 클라이언트 목록을 순회하며 메시지 전송
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (strcmp(sender, clnt_socks[i].username) != 0) // 메시지를 보낸 클라이언트를 제외
        {
            write(clnt_socks[i].sock, msg, len);
        }
    }

    pthread_mutex_unlock(&mutx);
}

void *server_input(void *arg)
{
    char msg[BUF_SIZE];
    while (1)
    {
        // 서버에서 입력을 받아서 전체 클라이언트에게 전송
        if (fgets(msg, sizeof(msg), stdin) != NULL)
        {
            char formatted_msg[BUF_SIZE + 50];
            snprintf(formatted_msg, sizeof(formatted_msg), "%s[공지] %s%s\n", RED_BOLD, msg, END);
            send_msg(formatted_msg, strlen(formatted_msg), "Server");
        }
    }
    return NULL;
}

/* 에러 메시지를 출력하고 프로그램을 종료하는 함수 */
void error_handling(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

/* 사용자 인증을 수행하는 함수 */
int is_user_valid(const char *username, const char *password)
{
    FILE *file = fopen(USR_FILE, "r");
    if (!file)
    {
        perror("fopen() error");
        return 0;
    }

    char file_username[BUF_SIZE];
    char file_password[BUF_SIZE];

    while (fscanf(file, "%s %s", file_username, file_password) != EOF)
    {
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* 새 사용자를 등록하는 함수 */
void register_user(const char *username, const char *password)
{
    FILE *file = fopen(USR_FILE, "a");
    if (!file)
    {
        perror("fopen() error");
        return;
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
}

/* 사용자가 이미 존재하는지 확인하는 함수 */
int is_user_exists(const char *username)
{
    FILE *file = fopen(USR_FILE, "r");
    if (!file)
    {
        perror("fopen() error");
        return 0;
    }

    char file_username[BUF_SIZE];
    char file_password[BUF_SIZE];

    while (fscanf(file, "%s %s", file_username, file_password) != EOF)
    {
        if (strcmp(username, file_username) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* 사용자가 이미 로그인되어 있는지 확인하는 함수 */
int is_user_logged_in(const char *username)
{
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (strcmp(clnt_socks[i].username, username) == 0)
        {
            return clnt_socks[i].loggedIn;
            // return 1;
        }
    }
    return 0;
}

/* 쪽지함 기존 정보 저장하기 */
int user_Note(const char *username)
{
    FILE *file = fopen(USR_NOTE, "r");
    if (!file)
    {
        perror("fopen() error");
        return 0;
    }

    char file_sender[BUF_SIZE];
    char file_receiver[BUF_SIZE];
    char file_content[NOTE_SIZE];
    int file_readYN;
    int file_deleteYN;
    char file_sendTime[BUF_SIZE];

    int file_index = 0;

    while (fscanf(file, "%s %s %s %d %d %s", file_sender, file_receiver, file_content, &file_readYN, &file_deleteYN, file_sendTime) != EOF)
    {
        if (strcmp(username, file_receiver) == 0)
        {
            strcpy(note_list[file_index].sender, file_sender);
            strcpy(note_list[file_index].receiver, file_receiver);
            strcpy(note_list[file_index].content, file_content);
            note_list[file_index].readYN = file_readYN;
            note_list[file_index].deleteYN = file_deleteYN;

            // sendTime을 동적 할당하고 문자열 복사
            note_list[file_index].sendTime = strdup(file_sendTime);
            if (note_list[file_index].sendTime == NULL)
            {
                perror("Memory allocation error for sendTime");
                fclose(file);
                return 0;
            }
        }
        file_index++;
    }

    fclose(file);
    return 1;
}