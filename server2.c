#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define MAX_ATTEMPTS 3

typedef struct {
    int sock;
    char username[BUF_SIZE];
} ClientInfo;

// 사용자 인증 함수
int authenticate_user(const char *username, const char *password) {
    // 실제 인증 로직으로 교체
    return strcmp(username, "user") == 0 && strcmp(password, "pass") == 0;
}

// 사용자 등록 함수
void register_user(const char *username, const char *password) {
    // 실제 등록 로직으로 교체
    printf("Registered user: %s with password: %s\n", username, password);
}

// 클라이언트 처리 함수
void *handle_client(void *arg) {
    ClientInfo *client_info = (ClientInfo *)arg;
    int client_sock = client_info->sock;
    char msg[BUF_SIZE];
    int attempts_left = MAX_ATTEMPTS;
    ssize_t str_len;

    while (1) {
        memset(msg, 0, sizeof(msg));
        str_len = read(client_sock, msg, sizeof(msg) - 1);
        if (str_len <= 0) { // 연결 종료 또는 오류 처리
            close(client_sock);
            return NULL;
        }

        msg[str_len] = '\0'; // 수신된 문자열의 널 종료
        if (strncmp(msg, "1", 1) == 0) { // 로그인
            char username[BUF_SIZE], password[BUF_SIZE];
            sscanf(msg + 2, "%s %s", username, password);

            if (authenticate_user(username, password)) {
                const char *login_success_msg = "Login successful.\n";
                write(client_sock, login_success_msg, strlen(login_success_msg));
                break; // 성공적인 로그인 후 루프 종료
            } else {
                attempts_left--;
                if (attempts_left > 0) {
                    char login_failed_msg[BUF_SIZE];
                    snprintf(login_failed_msg, sizeof(login_failed_msg), "Login failed. Attempts remaining: %d\n", attempts_left);
                    write(client_sock, login_failed_msg, strlen(login_failed_msg));
                } else {
                    const char *no_attempts_left_msg = "Login failed. No attempts left.\n";
                    write(client_sock, no_attempts_left_msg, strlen(no_attempts_left_msg));
                    close(client_sock);
                    return NULL;
                }
            }
        } else if (strncmp(msg, "2", 1) == 0) { // 회원 가입
            char username[BUF_SIZE], password[BUF_SIZE];
            sscanf(msg + 2, "%s %s", username, password);

            register_user(username, password);
            const char *registration_success_msg = "Registration successful. You can now log in.\n";
            write(client_sock, registration_success_msg, strlen(registration_success_msg));
        } else if (strncmp(msg, "0", 1) == 0) { // 종료
            close(client_sock);
            return NULL;
        } else {
            const char *invalid_option_msg = "Invalid option. Please try again.\n";
            write(client_sock, invalid_option_msg, strlen(invalid_option_msg));
        }
    }

    // 로그인 후 채팅 처리
    while (1) {
        memset(msg, 0, sizeof(msg));
        str_len = read(client_sock, msg, sizeof(msg) - 1);
        if (str_len <= 0) {
            close(client_sock);
            return NULL;
        }

        msg[str_len] = '\0'; // 수신된 문자열의 널 종료
        // 메시지 브로드캐스트 또는 채팅 기능 처리
        write(client_sock, msg, str_len); // 수신된 메시지를 클라이언트에 다시 전송
    }
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

    printf("Server is running on port %s\n", argv[1]);

    while (1) {
        client_addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_sock == -1) {
            perror("accept() error");
            continue;
        }

        ClientInfo *client_info = (ClientInfo *)malloc(sizeof(ClientInfo));
        client_info->sock = client_sock;

        if (pthread_create(&t_id, NULL, handle_client, (void *)client_info) != 0) {
            perror("pthread_create() error");
            close(client_sock);
            free(client_info);
            continue;
        }
        pthread_detach(t_id);
    }

    close(server_sock);
    return 0;
}
