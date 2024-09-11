#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define MAX_CLIENTS 100
#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

typedef struct {
    int sock;
    char username[MAX_USERNAME_LENGTH + 1];
    int logged_in;
} Client;

Client *clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    char username[MAX_USERNAME_LENGTH + 1];
    char password[MAX_PASSWORD_LENGTH + 1];
} User;

#define MAX_USERS 100
User users[MAX_USERS];
int user_count = 0;

int is_username_taken(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return 1;
        }
    }
    return 0;
}

void add_user(const char *username, const char *password) {
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;
}

void broadcast_message(const char *message, Client *exclude_client) {
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] != exclude_client && clients[i]->logged_in) {
            write(clients[i]->sock, message, strlen(message));
        }
    }
    pthread_mutex_unlock(&client_mutex);
}

void *client_handler(void *arg) {
    Client *client = (Client *)arg;
    char buf[BUF_SIZE];
    int str_len;

    while ((str_len = read(client->sock, buf, sizeof(buf) - 1)) != 0) {
        if (str_len <= 0) {
            break;
        }

        buf[str_len] = '\0';
        int choice;
        char username[MAX_USERNAME_LENGTH + 1], password[MAX_PASSWORD_LENGTH + 1];

        // 클라이언트 요청 파싱
        if (sscanf(buf, "%d %100s %100s", &choice, username, password) != 3) {
            printf("잘못된 요청 형식입니다.\n");
            continue;
        }

        if (choice == 2) { // 회원 가입
            if (is_username_taken(username)) {
                write(client->sock, "회원 가입 실패: 아이디가 이미 존재합니다.", strlen("회원 가입 실패: 아이디가 이미 존재합니다."));
            } else {
                add_user(username, password);
                write(client->sock, "회원 가입 성공", strlen("회원 가입 성공"));
            }
        } else if (choice == 1) { // 로그인
            int login_success = 0;
            for (int i = 0; i < user_count; i++) {
                if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
                    login_success = 1;
                    strcpy(client->username, username);
                    client->logged_in = 1;
                    break;
                }
            }

            if (login_success) {
                write(client->sock, "로그인 성공", strlen("로그인 성공"));
                char login_message[BUF_SIZE];
                snprintf(login_message, sizeof(login_message), "%s님이 로그인했습니다.", client->username);
                broadcast_message(login_message, client);
            } else {
                write(client->sock, "로그인 실패: 아이디 또는 비밀번호가 잘못되었습니다.", strlen("로그인 실패: 아이디 또는 비밀번호가 잘못되었습니다."));
            }
        }
    }

    // 클라이언트 종료 시 자원 정리
    close(client->sock);
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client) {
            clients[i] = clients[client_count - 1];
            break;
        }
    }
    client_count--;
    pthread_mutex_unlock(&client_mutex);
    free(client);
    return NULL;
}

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    pthread_t t_id;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind() error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) == -1) {
        perror("listen() error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("서버가 시작되었습니다.\n");

    while (1) {
        client_addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_sock == -1) {
            perror("accept() error");
            continue;
        }

        Client *client = (Client *)malloc(sizeof(Client));
        client->sock = client_sock;
        client->logged_in = 0;

        pthread_mutex_lock(&client_mutex);
        clients[client_count++] = client;
        pthread_mutex_unlock(&client_mutex);

        pthread_create(&t_id, NULL, client_handler, (void *)client);
        pthread_detach(t_id);

        printf("클라이언트가 연결되었습니다.\n");
    }

    close(server_sock);
    return 0;
}

/*
회원가입 시 아이디가 이미 존재할 경우: 서버에서 기존 아이디 체크를 정확하게 수행하고, 이미 존재할 때는 "회원 가입 실패" 메시지를 클라이언트에 전달.
로그인 절차 개선: 로그인 성공 시 logged_in 플래그를 설정하여 클라이언트가 로그인 상태로 올바르게 인식되도록 설정.
회원가입, 로그인 실패 시 재입력 가능: 실패 메시지를 받은 후 클라이언트가 다시 입력을 받을 수 있도록 흐름을 조정.
*/

/*

변경 사항 설명
클라이언트 목록 관리: clients 배열을 사용하여 연결된 클라이언트 소켓을 관리합니다. add_client()와 remove_client() 함수를 사용하여 클라이언트 소켓을 추가하고 제거합니다.

메시지 브로드캐스트: broadcast_message() 함수를 사용하여 모든 클라이언트에게 메시지를 브로드캐스트합니다. 특정 클라이언트(메시지를 보낸 클라이언트)는 제외하고 브로드캐스트합니다.

클라이언트 스레드 처리: handle_client() 함수에서 클라이언트의 메시지를 처리하고, 로그인 후에는 채팅 메시지를 처리합니다.

스레드 관리: pthread_create()로 클라이언트 처리를 위한 스레드를 생성하고, pthread_detach()로 스레드가 종료되면 자동으로 자원이 해제되도록 합니다.
메시지 크기 조절:

snprintf 호출 후 메시지의 크기를 확인하고, 버퍼 크기를 초과할 경우 잘라냅니다.
클라이언트 목록 관리:

클라이언트 목록에 추가 및 제거하는 기능을 추가하여 모든 클라이언트에게 메시지를 브로드캐스트할 수 있도록 합니다.
broadcast_message() 함수:

클라이언트 목록을 순회하며 메시지를 전송합니다. 제외할 클라이언트 소켓을 필터링하여 메시지를 자신에게 보내지 않도록 합니다.
*/