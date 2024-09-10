#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define USER_FILE "users.txt"

void *handle_clnt(void *arg);
void send_msg(const char *msg, int len);
void error_handling(const char *msg);
int authenticate_user(const char *username, const char *password);
void register_user(const char *username, const char *password);
int user_exists(const char *username);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while (1) {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        if (clnt_sock == -1) {
            perror("accept() failed");
            continue;
        }

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s\n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int*)arg);
    int str_len = 0;
    char msg[BUF_SIZE];
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    int is_authenticated = 0;

    // Initial login/register prompt
    const char *welcome_msg = "Welcome! Please log in or register.\n";
    write(clnt_sock, welcome_msg, strlen(welcome_msg));

    while ((str_len = read(clnt_sock, msg, sizeof(msg) - 1)) > 0) {
        msg[str_len] = '\0'; // Ensure null termination

        if (!is_authenticated) {
            if (strncmp(msg, "register", 8) == 0) {
                // Extract username and password
                sscanf(msg + 9, "%s %s", username, password);
                if (user_exists(username)) {
                    const char *error_msg = "Username already exists.\n";
                    write(clnt_sock, error_msg, strlen(error_msg));
                } else {
                    register_user(username, password);
                    const char *success_msg = "Registration successful.\n";
                    write(clnt_sock, success_msg, strlen(success_msg));
                }
            } else if (strncmp(msg, "login", 5) == 0) {
                // Extract username and password
                sscanf(msg + 6, "%s %s", username, password);
                if (authenticate_user(username, password)) {
                    is_authenticated = 1;
                    const char *success_msg = "Login successful.\n";
                    write(clnt_sock, success_msg, strlen(success_msg));
                } else {
                    const char *error_msg = "Login failed. Try again.\n";
                    write(clnt_sock, error_msg, strlen(error_msg));
                }
            } else {
                const char *error_msg = "Please log in or register first.\n";
                write(clnt_sock, error_msg, strlen(error_msg));
            }
        } else {
            // Broadcast messages if authenticated
            send_msg(msg, str_len);
        }
    }

    // Client disconnected
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++) {
        if (clnt_sock == clnt_socks[i]) {
            while (i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);

    // Notify all clients that a client has disconnected
    snprintf(msg, sizeof(msg), "A client has disconnected.\n");
    send_msg(msg, strlen(msg));

    close(clnt_sock);
    return NULL;
}

void send_msg(const char *msg, int len)
{
    int i;
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

int authenticate_user(const char *username, const char *password)
{
    FILE *file = fopen(USER_FILE, "r");
    char line[BUF_SIZE];
    char file_user[BUF_SIZE], file_pass[BUF_SIZE];

    if (file == NULL)
        return 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%s %s", file_user, file_pass);
        if (strcmp(username, file_user) == 0 && strcmp(password, file_pass) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void register_user(const char *username, const char *password)
{
    FILE *file = fopen(USER_FILE, "a");
    if (file == NULL)
        error_handling("Failed to open user file");

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
}

int user_exists(const char *username)
{
    FILE *file = fopen(USER_FILE, "r");
    char line[BUF_SIZE];
    char file_user[BUF_SIZE];

    if (file == NULL)
        return 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%s", file_user);
        if (strcmp(username, file_user) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}