#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 256

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_adr;
    char msg[BUF_SIZE];
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    int is_logged_in = 0;

    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
        perror("connect() error");
        exit(1);
    }

    while (1) {
        if (!is_logged_in) {
            printf("원하는 작업을 선택하세요 (1: 로그인, 2: 회원 가입, 0: 종료): ");
            fgets(msg, sizeof(msg), stdin);
            msg[strlen(msg) - 1] = 0; // Remove newline character

            if (strcmp(msg, "0") == 0) {
                close(sock);
                exit(0);
            }

            if (strcmp(msg, "1") == 0) {
                printf("사용자 이름과 비밀번호를 입력하세요: ");
                fgets(msg, sizeof(msg), stdin);
                msg[strlen(msg) - 1] = 0; // Remove newline character
                write(sock, msg, strlen(msg));

                int str_len = read(sock, msg, sizeof(msg) - 1);
                msg[str_len] = 0;
                printf("%s", msg);

                if (strstr(msg, "Login successful") != NULL) {
                    is_logged_in = 1; // Set login state to true
                }

            } else if (strcmp(msg, "2") == 0) {
                printf("회원 가입 정보를 입력하세요 (형식: username password): ");
                fgets(msg, sizeof(msg), stdin);
                msg[strlen(msg) - 1] = 0; // Remove newline character
                write(sock, msg, strlen(msg));

                int str_len = read(sock, msg, sizeof(msg) - 1);
                msg[str_len] = 0;
                printf("%s", msg);

            } else {
                printf("Invalid option. Please try again.\n");
            }
        } else {
            printf("메시지를 입력하세요 (형식: msg <message>): ");
            fgets(msg, sizeof(msg), stdin);
            msg[strlen(msg) - 1] = 0; // Remove newline character
            write(sock, msg, strlen(msg));

            int str_len = read(sock, msg, sizeof(msg) - 1);
            msg[str_len] = 0;
            printf("서버로부터의 응답: %s\n", msg);
        }
    }

    close(sock);
    return 0;
}
