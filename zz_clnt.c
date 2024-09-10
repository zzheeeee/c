#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 100

void error_handling(const char *msg);
void send_message(int sock, const char *message);
void receive_message(int sock);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    ssize_t str_len;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    while (1) {
        // Display menu
        printf("1. Register\n2. Login\n3. Exit\n");
        printf("Select an option: ");
        if (fgets(message, sizeof(message), stdin) == NULL)
            error_handling("fgets() error");
        message[strcspn(message, "\n")] = '\0';  // Remove newline character

        if (strcmp(message, "1") == 0) {
            // Register
            printf("Enter username: ");
            if (fgets(username, sizeof(username), stdin) == NULL)
                error_handling("fgets() error");
            username[strcspn(username, "\n")] = '\0';  // Remove newline character

            printf("Enter password: ");
            if (fgets(password, sizeof(password), stdin) == NULL)
                error_handling("fgets() error");
            password[strcspn(password, "\n")] = '\0';  // Remove newline character

            int len = snprintf(message, sizeof(message), "register %s %s", username, password);
            if (len < 0 || len >= sizeof(message))
                error_handling("snprintf() error or buffer overflow");

            send_message(sock, message);
        }
        else if (strcmp(message, "2") == 0) {
            // Login
            printf("Enter username: ");
            if (fgets(username, sizeof(username), stdin) == NULL)
                error_handling("fgets() error");
            username[strcspn(username, "\n")] = '\0';  // Remove newline character

            printf("Enter password: ");
            if (fgets(password, sizeof(password), stdin) == NULL)
                error_handling("fgets() error");
            password[strcspn(password, "\n")] = '\0';  // Remove newline character

            int len = snprintf(message, sizeof(message), "login %s %s", username, password);
            if (len < 0 || len >= sizeof(message))
                error_handling("snprintf() error or buffer overflow");

            send_message(sock, message);

            // Receive response from the server
            receive_message(sock);
        }
        else if (strcmp(message, "3") == 0) {
            // Exit
            int len = snprintf(message, sizeof(message), "exit");
            if (len < 0 || len >= sizeof(message))
                error_handling("snprintf() error or buffer overflow");

            send_message(sock, message);
            break;
        }
        else {
            printf("Invalid option. Please try again.\n");
        }

        // Receive and print the server's response
        receive_message(sock);
    }

    close(sock);
    return 0;
}

void send_message(int sock, const char *message)
{
    ssize_t sent_bytes = write(sock, message, strlen(message));
    if (sent_bytes == -1)
        error_handling("write() error");
    if (sent_bytes < (ssize_t)strlen(message))
        fprintf(stderr, "Warning: Partial write occurred. Sent %ld of %zu bytes.\n", sent_bytes, strlen(message));
}

void receive_message(int sock)
{
    char message[BUF_SIZE];
    ssize_t str_len = read(sock, message, sizeof(message) - 1);
    if (str_len == -1)
        error_handling("read() error");
    if (str_len == 0) {
        printf("Server disconnected.\n");
        exit(1);
    }

    message[str_len] = '\0';  // Null-terminate the string
    printf("Server: %s\n", message);
}

void error_handling(const char *msg)
{
    perror(msg);
    exit(1);
}