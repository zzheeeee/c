#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 256
#define MAX_CLIENTS 10
#define MAX_ATTEMPTS 3

typedef struct {
    char username[BUF_SIZE];
    char password[BUF_SIZE];
} User;

User users[MAX_CLIENTS];
int user_count = 0;

void register_user(const char* username, const char* password) {
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0) {
            return; // User already exists
        }
    }
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;
}

int authenticate_user(const char* username, const char* password) {
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            return 1; // Authentication successful
        }
    }
    return 0; // Authentication failed
}

void handle_client(int client_sock) {
    char msg[BUF_SIZE];
    int attempts_left = MAX_ATTEMPTS;
    ssize_t str_len;

    while (1) {
        memset(msg, 0, sizeof(msg));
        str_len = read(client_sock, msg, sizeof(msg) - 1);
        if (str_len <= 0) { // 연결 종료 또는 오류 처리
            close(client_sock);
            return;
        }

        msg[str_len] = '\0'; // Null-terminate the received string

        if (strncmp(msg, "1", 1) == 0) { // Login
            char username[BUF_SIZE], password[BUF_SIZE];
            sscanf(msg + 2, "%s %s", username, password);

            if (authenticate_user(username, password)) {
                const char *login_success_msg = "Login successful.\n";
                write(client_sock, login_success_msg, strlen(login_success_msg));
                break; // Exit the loop on successful login
            } else {
                attempts_left--;
                if (attempts_left > 0) {
                    char login_failed_msg[BUF_SIZE];
                    snprintf(login_failed_msg, sizeof(login_failed_msg), "Login failed. Attempts remaining: %d\n", attempts_left);
                    write(client_sock, login_failed_msg, strlen(login_failed_msg)); // Use strlen to get correct size
                } else {
                    const char *no_attempts_left_msg = "Login failed. No attempts left.\n";
                    write(client_sock, no_attempts_left_msg, strlen(no_attempts_left_msg));
                    close(client_sock);
                    return;
                }
            }
        } else if (strncmp(msg, "2", 1) == 0) { // Register
            char username[BUF_SIZE], password[BUF_SIZE];
            sscanf(msg + 2, "%s %s", username, password);

            register_user(username, password);
            const char *registration_success_msg = "Registration successful. You can now log in.\n";
            write(client_sock, registration_success_msg, strlen(registration_success_msg));
        } else if (strncmp(msg, "0", 1) == 0) { // Exit
            close(client_sock);
            return;
        } else {
            const char *invalid_option_msg = "Invalid option. Please try again.\n";
            write(client_sock, invalid_option_msg, strlen(invalid_option_msg));
        }
    }

    // Handle the chat after successful login
    while (1) {
        memset(msg, 0, sizeof(msg));
        str_len = read(client_sock, msg, sizeof(msg) - 1);
        if (str_len <= 0) {
            close(client_sock);
            return;
        }

        msg[str_len] = '\0'; // Null-terminate the received string
        // Broadcast message or handle chat functionality
        write(client_sock, msg, str_len); // Use str_len to get correct size
    }
}

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    int option = 1;

    if (argc != 2) {
        printf("Usage: %s <PORT>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        perror("socket() error");
        exit(1);
    }

    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("setsockopt() error");
        exit(1);
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
        perror("bind() error");
        exit(1);
    }

    if (listen(serv_sock, 5) == -1) {
        perror("listen() error");
        exit(1);
    }

    while (1) {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1) {
            perror("accept() error");
            continue;
        }

        handle_client(clnt_sock);
    }

    close(serv_sock);
    return 0;
}
