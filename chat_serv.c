#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100              // 메시지와 파일 버퍼의 크기 
#define MAX_CLNT 256              // 서버가 동시에 처리할 수 있는 최대 클라이언트 수
#define USER_FILE "users.txt"     // 사용자 정보를 저장할 파일명

typedef struct {
    int sock;
    char username[BUF_SIZE];
} ClientInfo;                     // 현재 클라이언트 소켓번호, 이름 저장 구조체

int clnt_cnt = 0;                 // 현재 연결된 클라이언트의 수
ClientInfo clnt_socks[MAX_CLNT];  // 연결된 클라이언트의 소켓 파일 디스크립터
pthread_mutex_t mutx;             // 클라이언트 소켓 배열에 대한 동시 접근을 동기화하기 위한 뮤텍스

void *handle_clnt(void *arg);            // 클라이언트와의 연결을 처리하는 쓰레드의 메인 함수
void send_msg(const char *msg, const char *sender); // 모든 클라이언트에게 메시지를 전송하는 함수
void error_handling(const char *msg);    // 에러 메시지를 출력하고 프로그램을 종료하는 함수

int authenticate_user(const char *username, const char *password); // 사용자 인증을 수행하는 함수
void register_user(const char *username, const char *password);    // 새 사용자를 등록하는 함수
int user_exists(const char *username);   // 사용자가 이미 존재하는지 확인하는 함수
int is_user_logged_in(const char *username); // 사용자가 이미 로그인되어 있는지 확인하는 함수

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

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
        clnt_socks[clnt_cnt].sock = clnt_sock;
        clnt_socks[clnt_cnt].username[0] = '\0'; // 초기화
        clnt_cnt++;
        pthread_mutex_unlock(&mutx);

        /* 새로운 쓰레드 생성하여 handle_clnt 함수 실행 */
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
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
    int is_authenticated = 0;

    const char *welcome_msg = "1. 로그인 2. 회원 가입\n";
    if (write(clnt_sock, welcome_msg, strlen(welcome_msg)) == -1)
    {
        perror("write() error");
        close(clnt_sock);
        return NULL;
    }

    while ((str_len = read(clnt_sock, msg, sizeof(msg) - 1)) != 0)
    {
        if (str_len == -1)
        {
            perror("read() error");
            close(clnt_sock);
            return NULL;
        }
        msg[str_len] = 0;

        if (strncmp(msg, "/user", 5) == 0)
        {
            // 클라이언트 목록 전송
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
        else if (!is_authenticated) 
        {
            if (strncmp(msg, "register", 8) == 0)
            {
                sscanf(msg + 9, "%s %s", username, password);
                if (user_exists(username))
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
                if (authenticate_user(username, password))
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
                        for (int i = 0; i < clnt_cnt; i++)
                        {
                            if (clnt_socks[i].sock == clnt_sock)
                            {
                                strcpy(clnt_socks[i].username, username);
                                break;
                            }
                        }
                        pthread_mutex_unlock(&mutx);
                        const char *login_success_msg = "로그인 성공\n";
                        if (write(clnt_sock, login_success_msg, strlen(login_success_msg)) == -1)
                        {
                            perror("write() error");
                            close(clnt_sock);
                            return NULL;
                        }
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
        }
        else if (is_authenticated)
        {
            pthread_mutex_lock(&mutx);
            char full_msg[BUF_SIZE];
            
            // 최대 길이를 고려하여 버퍼를 초과하지 않도록 조정
            size_t username_len = strlen(clnt_socks[clnt_cnt - 1].username);
            size_t msg_len = strlen(msg);
            size_t max_len = sizeof(full_msg) - 1; // null terminator 고려
            int snprintf_len = snprintf(full_msg, max_len, "[%s]: %s", clnt_socks[clnt_cnt - 1].username, msg);
            
            // snprintf가 생성한 문자열이 버퍼를 초과하는지 확인하고 필요한 만큼만 사용
            if (snprintf_len >= max_len) {
                full_msg[max_len - 1] = '\0'; // 버퍼 크기를 초과하는 경우, 마지막에 null terminator 추가
            }
            
            send_msg(full_msg, clnt_socks[clnt_cnt - 1].username);
            pthread_mutex_unlock(&mutx);
        }
        else
        {
            const char *auth_prompt_msg = "로그인 후 메시지를 보낼 수 있습니다.\n";
            if (write(clnt_sock, auth_prompt_msg, strlen(auth_prompt_msg)) == -1)
            {
                perror("write() error");
                close(clnt_sock);
                return NULL;
            }
        }
    }

    /* 클라이언트가 연결을 종료하면 클라이언트 소켓을 닫고 배열에서 제거 */
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
    return NULL;
}

/* 모든 클라이언트에게 메시지를 전송하는 함수 */
void send_msg(const char *msg, const char *sender)
{
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
    {
        char full_msg[BUF_SIZE];
        if (strcmp(clnt_socks[i].username, sender) == 0)
        {
            // 내가 보낸 메시지에는 파란색 ANSI 코드 추가
            snprintf(full_msg, sizeof(full_msg), "\033[94m%s\033[0m", msg);
        }
        else
        {
            snprintf(full_msg, sizeof(full_msg), "%s", msg);
        }
        
        if (write(clnt_socks[i].sock, full_msg, strlen(full_msg)) == -1)
        {
            perror("write() error");
            close(clnt_socks[i].sock);
        }
    }
    pthread_mutex_unlock(&mutx);
}

/* 에러 메시지를 출력하고 프로그램을 종료하는 함수 */
void error_handling(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

/* 사용자 인증을 수행하는 함수 */
int authenticate_user(const char *username, const char *password)
{
    FILE *file = fopen(USER_FILE, "r");
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
    FILE *file = fopen(USER_FILE, "a");
    if (!file)
    {
        perror("fopen() error");
        return;
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
}

/* 사용자가 이미 존재하는지 확인하는 함수 */
int user_exists(const char *username)
{
    FILE *file = fopen(USER_FILE, "r");
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
            return 1;
        }
    }
    return 0;
}
