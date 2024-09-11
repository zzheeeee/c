#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

void handle_server_response(int sock) {
    char buf[BUF_SIZE];
    int str_len;

    while ((str_len = read(sock, buf, sizeof(buf) - 1)) > 0) {
        buf[str_len] = '\0'; // Null-terminate the received string
        printf("%s", buf);
    }

    if (str_len < 0) {
        perror("read() error");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZE];
    char username[MAX_USERNAME_LENGTH + 1], password[MAX_PASSWORD_LENGTH + 1];
    int choice;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Welcome to the chat server. Please login or register.\n");
        printf("원하는 작업을 선택하세요 (1: 로그인, 2: 회원 가입, 0: 종료): ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            continue;
        }
        getchar(); // To consume newline character left by scanf

        if (choice == 1 || choice == 2) {
            printf("사용자 이름과 비밀번호를 입력하세요: ");
            if (scanf("%100s %100s", username, password) != 2) {
                printf("Invalid input.\n");
                continue;
            }
            getchar(); // To consume newline character left by scanf

            // Create message to send to the server
            snprintf(buf, sizeof(buf), "%d %s %s", choice, username, password);
            if (write(sock, buf, strlen(buf)) == -1) {
                perror("write() error");
                close(sock);
                exit(EXIT_FAILURE);
            }
        } else if (choice == 0) {
            // Send exit signal to server
            if (write(sock, "0", 1) == -1) {
                perror("write() error");
                close(sock);
                exit(EXIT_FAILURE);
            }
            close(sock);
            exit(EXIT_SUCCESS);
        } else {
            printf("잘못된 옵션입니다. 다시 시도하세요.\n");
            continue;
        }

        handle_server_response(sock);
    }

    close(sock);
    return 0;
}
