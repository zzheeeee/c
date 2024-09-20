#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <locale.h> // setlocale 함수를 사용하기 위해!

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
    int roomnumber;          // 현재 채팅방번호
} Client;

/* 채팅방 구조체 정의 */
typedef struct
{
    char username[BUF_SIZE]; // 방장
    char roomname[BUF_SIZE]; // 방이름
    int usercount;           // 인원
    int roomnumber;          // 방번호

} ChatRoom;

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

/* 전역 변수 */
pthread_mutex_t mutx;               // 클라이언트 소켓 배열에 대한 동시 접근을 동기화하기 위한 뮤텍스
int clnt_cnt = 0;                   // 현재 연결된 클라이언트의 수
Client clients[MAX_CLNT];           // 연결된 클라이언트의 소켓 파일 디스크립터
ChatRoom chatrms[11];               // 생성된 채팅방
char all_users[MAX_CLNT][BUF_SIZE]; // 등록된 전체 사용자 이름 [MAX_CLNT]:배열에 저장할 수 있는 최대 사용자, [BUF_SIZE]:각 사용자 이름의 최대 길이
int all_users_len;                  // 저장된 사용자 수
Note note_list[NOTE_SIZE];          // 쪽지 리스트(전체)
Note user_note_list[NOTE_SIZE];     // 쪽지 리스트(사용자)
int note_count = 0;                 // 현재 저장된 쪽지의 수
int currentroom = 0;                // 현재 사용중인 채팅방 수

/* 함수 프로토타입 선언 */
void *server_input(void *arg);                                  // 메시지 > 서버 공지
void error_handling(const char *msg);                           // 메시지 > 에러 메시지를 출력하고 프로그램을 종료하는 함수
void send_msg(const char *msg, int len, const char *sender);    // 메시지 > 모든 클라이언트에게 메시지를 전송하는 함수
void *handle_clnt(void *arg);                                   // 클라이언트와의 연결을 처리하는 쓰레드의 메인 함수
void remove_client(int clnt_sock);                              // 클라이언트 소켓 제거
void register_user(const char *username, const char *password); // 신규 사용자 등록
int is_user_valid(const char *username, const char *password);  // 로그인 정보 인증
int is_user_exists(const char *username);                       // 사용자가 이미 존재하는지 확인
int is_user_logged_in(const char *username);                    // 사용자가 이미 로그인되어 있는지 확인
int users_read();                                               // 등록된 전체 사용자 이름 가져오기
void initialize_note_list(Note note_list[], int size);          // 쪽지 구조체 초기화
void load_notes();                                              // 쪽지 텍스트파일 내용 저장하기
int note_check(const char *username);                           // 사용자의 쪽지함 확인
void process_update(const char *read, const char *delete);
void add_note_to_file(Note *note);
int unoccupiedNum();

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id, input_thread;

    setlocale(LC_ALL, "");

    /* 프로그램 실행 시 포트 번호가 인수로 제공되지 않으면 사용법을 출력하고 종료 */
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

        /* 클라이언트 연결 정보 초기화 */
        pthread_mutex_lock(&mutx);
        int idx = clnt_cnt++; // 현재 클라이언트의 인덱스를 저장 및 증가
        clients[idx].sock = clnt_sock;
        clients[idx].username[0] = '\0'; // 사용자 이름 초기화
        clients[idx].loggedIn = 0;       // 로그인 상태 초기화
        clients[idx].roomnumber = 0;     // 채팅방 번호 초기화

        /* 전체 사용자 길이, 쪽지 내용 가져오기 */
        all_users_len = users_read();
        initialize_note_list(note_list, NOTE_SIZE);
        load_notes();
        // 테스트 프린트
        // for (int i = 0; i < note_count; i++)
        // {
        //     printf("Index: %d\n", note_list[i].index);
        //     printf("Sender: %s\n", note_list[i].sender);
        //     printf("Receiver: %s\n", note_list[i].receiver);
        //     printf("Content: %s\n", note_list[i].content);
        //     printf("ReadYN: %d\n", note_list[i].readYN);
        //     printf("DeleteYN: %d\n", note_list[i].deleteYN);
        //     printf("SendTime: %s\n", note_list[i].sendTime);
        //     printf("----------------------------\n");
        // }
        printf("사용자 길이: %d, note_count : %d\n", all_users_len, note_count);

        strcpy(chatrms[0].roomname, "전체 채팅방");
        char numstr[10] = {0};
        for (int i = 0; i < 11; i++)
        {
            sprintf(numstr, "%d", i); // 숫자를 문자열로 변환

            // "방" + numstr 결합
            strcpy(chatrms[i].roomname, "방");   // "방"을 먼저 복사
            strcat(chatrms[i].roomname, numstr); // numstr을 붙임

            chatrms[i].roomnumber = i;
            chatrms[i].usercount = 0;
        }

        pthread_mutex_unlock(&mutx);

        /* 새로운 쓰레드 생성하여 handle_clnt 함수 실행 */
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clients[idx].sock);
        pthread_detach(t_id); // 쓰레드가 종료될 때 자원을 자동으로 반환
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
        if (clients[i].sock == clnt_sock)
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
                    strcpy(clients[idx].username, username);
                    clients[idx].loggedIn = 1; // 로그인 상태 설정
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
                    snprintf(login_msg, sizeof(login_msg), "%s%s님이 로그인 했습니다. %s\n", PINK_BOLD, clients[idx].username, END);
                    send_msg(login_msg, strlen(login_msg), clients[idx].username);
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

        msg[str_len] = '\0'; // 메시지 마지막에 널 추가

        if (strncmp(msg, "/user", 5) == 0)
        {
            pthread_mutex_lock(&mutx);
            char user_list[BUF_SIZE * MAX_CLNT];
            strcpy(user_list, "현재 접속중인 클라이언트:\n");
            for (int i = 0; i < clnt_cnt; i++)
            {
                if (clients[i].loggedIn == 1)
                {
                    strcat(user_list, clients[i].username);
                    strcat(user_list, "\n");
                }
            }
            pthread_mutex_unlock(&mutx);
            write(clnt_sock, user_list, strlen(user_list));
        }
        else if (strcmp(msg, "/makeroom") == 0)
        {
            char make_msg[BUF_SIZE]; // 메시지 생성
            currentroom++;           // 방 번호를 1 증가시킴 (1부터 10까지 사용)

            // 현재 방 번호를 문자열로 변환
            char numstr[3];
            int rmnumber = unoccupiedNum();
            if(0 < rmnumber)
                snprintf(numstr, sizeof(numstr), "%d", rmnumber);

            // 메시지 생성
            char mkrmsg[20];
            snprintf(mkrmsg, sizeof(mkrmsg), "/makeroom %s", numstr);

            if (rmnumber >= 1 && rmnumber <= 10)
            {
                chatrms[rmnumber].usercount++; // 방 사용자 수 업데이트

                // 사용자 이름 저장 및 방 번호 할당
                strcpy(chatrms[rmnumber].username, username);
                for (int i = 0; i < clnt_cnt; i++)
                {
                    if (strcmp(clients[i].username, username) == 0) // 문자열 비교
                    {
                        clients[i].roomnumber = rmnumber;
                        send(clnt_sock, mkrmsg, strlen(mkrmsg), 0); // 메시지 전송
                        printf("방 %d이 생성되었습니다.\n", rmnumber);

                        snprintf(make_msg, sizeof(make_msg), "%s%s님이 방을 나갔습니다. %s\n", PINK_BOLD, username, END);
                        send(clients[i].sock, make_msg, strlen(make_msg), 0); // 채팅방에 남아있는 사람에게 방 나감 메시지 전송
                        break;
                    }
                }
            }
        }
        else if (strcmp(msg, "/leaveroom") == 0)
        {
            char leave_msg[BUF_SIZE]; // 메시지 생성
            char mkrmsg[20];
            char numstr[3];
            int num = 0;

            for (int i = 0; i < clnt_cnt; i++)
            {
                if (strcmp(clients[i].username, username) == 0) // 문자열 비교
                {
                    num = clients[i].roomnumber;
                    chatrms[num].usercount--;  // 채팅방 구조체 > 인원 1 감소
                    clients[i].roomnumber = 0; // 사용자 구조체 > 방번호 0
                    snprintf(numstr, sizeof(numstr), "%d", clients[i].roomnumber);
                    snprintf(mkrmsg, sizeof(mkrmsg), "/leaveroom %s", numstr);
                    send(clnt_sock, mkrmsg, strlen(mkrmsg), 0); // 메시지 전송
                    break;
                }
            }

            if (chatrms[num].usercount > 0)
            {
                for (int i = 0; i < clnt_cnt; i++)
                {
                    if (clients[i].roomnumber == num)
                    {
                        strcpy(chatrms[num].username, clients[i].username); // 남은 사람한테 방장 넘기기 ㅋㅋ
                        snprintf(leave_msg, sizeof(leave_msg), "%s%s님이 방을 나갔습니다. %s\n", PINK_BOLD, username, END);
                        send(clients[i].sock, leave_msg, strlen(leave_msg), 0); // 채팅방에 남아있는 사람에게 방 나감 메시지 전송
                    }
                }
            }
            currentroom--;
        }
        else if (strncmp(msg, "/inviteroom", 11) == 0)
        {
            char invite_msg[BUF_SIZE];
            // 메시지 생성!!
            char mkrmsg[20];
            char numstr[3];
            int rmnum = -1, innum = -1;
            
            for (int i = 0; i < clnt_cnt; i++)
            {
                if (clients[i].username == username)
                {
                    rmnum = i;
                    strcpy(numstr, atoi(clients[rmnum].roomnumber));
                    printf("초대할 방 번호: %s\n", numstr);
                }
            }

            // "/inviteroom" 이후의 문자열을 분리
            char *usr = strtok(msg + 12, " "); // "/inviteroom " 다음부터 분리
            if(rmnum != -1)
            {                    
                for (int i = 0; i < clnt_cnt; i++)
                {
                    if (clients[i].username == usr)
                    {
                        printf("초대할 사용자: %s\n", usr);
                        innum = i;
                        chatrms[clients[rmnum].roomnumber].usercount++;
                    }
                }
            }

            if (innum != -1 && rmnum != -1) // username이 잘 분리된 경우
            {
                // // 추가 작업 수행 (예: 초대 메시지 생성 등) // 이후 처리
                // snprintf(mkrmsg, sizeof(mkrmsg), "초대 메시지: %s", usr);
                clients[innum].roomnumber = clients[rmnum].roomnumber;

                snprintf(invite_msg, sizeof(invite_msg), "%s%s님이 초대되었습니다. %s\n", PINK_BOLD, clients[innum].username, END);
                send(clients[innum].sock, invite_msg, strlen(invite_msg), 0); // 초대당한사람한테 초대된거 알려주기
                send(clients[rmnum].sock, invite_msg, strlen(invite_msg), 0); // 초대한사람한테도 보내기
            }
            else if (innum == -1)
            {
                printf("사용자 이름을 찾을 수 없습니다.\n");
                break;
            }
        }
        else if (strcmp(msg, "/room") == 0)
        {
            for(int i = 1; i < 11; i++)
            {
                if(chatrms[i].usercount != 0)
                {
                    printf("=========================================\n");
                    printf("[방%d]\n",i);
                    printf("참여 인원: %d명", chatrms[i].usercount);
                    printf("=========================================\n");                   
                }
            }
        }
        else if (strncmp(msg, "/exit", 5) == 0)
        {
            // 클라이언트의 로그아웃 상태로 변경
            // clients[idx].loggedIn = 0;

            // 로그아웃 메시지 생성
            char exit_msg[BUF_SIZE + 50];
            snprintf(exit_msg, sizeof(exit_msg), "%s%s님이 로그아웃 했습니다. %s\n", PINK_BOLD, username, END);
            printf("로그아웃 메시지 생성: %s\n", exit_msg); // 로그아웃 메시지 확인

            // 다른 클라이언트들에게 로그아웃 메시지 전송
            send_msg(exit_msg, strlen(exit_msg), username); // 모든 클라이언트에게 전송

            // 성공 허락하기
            const char *logout_success_msg = "로그아웃 성공\n";
            if (write(clnt_sock, logout_success_msg, strlen(logout_success_msg)) == -1)
            {
                perror("write() error");
            }

            // 소켓 제거 및 종료 처리
            remove_client(clnt_sock);

            return NULL;
        }
        else if (strncmp(msg, "/note_send", 10) == 0) // 메시지가 /note_send로 시작하는지 확인
        {
            char buffer[1024];      // 수신 버퍼
            ssize_t bytes_received; // 수신 바이트 수

            /* msg의 복사본을 생성 */
            char *message_copy = strdup(msg);
            if (message_copy == NULL)
            {
                perror("strdup() error"); // strdup 실패 시 에러 메시지 출력
                continue;                 // 다음 루프로 이동
            }

            char *tokens[7];                         // 토큰을 저장할 배열
            int token_count = 0;                     // 토큰 수 카운트
            char *token = strtok(message_copy, " "); // 첫 번째 토큰 추출

            /*/ /note_send 명령어 이후 데이터 추출 */
            if (strncmp(token, "/note_send", 10) != 0)
            {
                fprintf(stderr, "Invalid command\n"); // 유효하지 않은 명령어 시 에러 메시지 출력
                free(message_copy);                   // 메모리 해제
                continue;                             // 다음 루프로 이동
            }

            // 나머지 토큰 추출
            while ((token = strtok(NULL, " ")) != NULL && token_count < 7)
            {
                tokens[token_count++] = token; // 토큰 저장
            }

            // 데이터가 충분하지 않으면 오류 메시지 출력
            if (token_count != 7)
            {
                fprintf(stderr, "Error: insufficient data\n"); // 데이터 부족 시 에러 메시지 출력
                free(message_copy);                            // 메모리 해제
                continue;                                      // 다음 루프로 이동
            }

            Note newNote;                        // 새로운 쪽지 구조체 선언
            newNote.index = atoi(tokens[0]);     // 인덱스 설정
            strcpy(newNote.sender, tokens[1]);   // 발신자 설정
            strcpy(newNote.receiver, tokens[2]); // 수신자 설정
            strcpy(newNote.content, tokens[3]);  // 내용 설정
            newNote.readYN = atoi(tokens[4]);    // 읽음 여부 설정
            newNote.deleteYN = atoi(tokens[5]);  // 삭제 여부 설정
            strcpy(newNote.sendTime, tokens[6]); // 전송 시간 설정

            // 파일에 데이터 추가
            add_note_to_file(&newNote); // 새로운 쪽지를 파일에 추가
            free(message_copy);         // 메모리 해제
        }
        else if (strncmp(msg, "/note_check", 11) == 0)
        {
            // 쪽지 목록 초기화
            initialize_note_list(user_note_list, NOTE_SIZE);

            // 쪽지 목록 확인 및 전송
            int len = note_check(username);
            int idx = 0;
            Note list[len];
            initialize_note_list(list, len);
            for (int i = 0; i < NOTE_SIZE; i++)
            {
                if (strcmp(user_note_list[i].receiver, username) == 0 && user_note_list[i].deleteYN == 0)
                {
                    list[idx].index = user_note_list[i].index;
                    strcpy(list[idx].sender, user_note_list[i].sender);
                    strcpy(list[idx].receiver, user_note_list[i].receiver);
                    strcpy(list[idx].content, user_note_list[i].content);
                    list[idx].readYN = user_note_list[i].readYN;
                    list[idx].deleteYN = user_note_list[i].deleteYN;
                    strcpy(list[idx].sendTime, user_note_list[i].sendTime);
                    // printf("DeleteYN: %d\n", list[idx].deleteYN);
                    // printf("ReadYN: %d\n", list[idx].readYN);
                    // printf("Content: %s\n", list[idx].content);
                    // printf("Receiver: %s\n", list[idx].receiver);
                    // printf("Sender: %s\n", list[idx].sender);
                    // printf("Index: %d\n", list[idx].index);
                    // printf("SendTime: %s\n", list[idx].sendTime);
                    idx++;
                }
            }
            // 전송할 데이터 버퍼 준비
            char buffer[1024];
            char message[1024];

            // 메시지 생성
            snprintf(message, sizeof(message), "note_check%ld", sizeof(list));
            send(clnt_sock, message, strlen(message), 0);
            sleep(2);
            // 구조체 배열 전송
            send(clnt_sock, list, sizeof(list), 0);
            sleep(2);
            printf("Data sent to client\n");
        }
        else if (strncmp(msg, "/w", 2) == 0) // 메시지가 "/w"로 시작하는 경우
        {
            char receiver[BUF_SIZE]; // 수신자 이름을 저장할 버퍼
            char message[BUF_SIZE];  // 메시지 내용을 저장할 버퍼

            // "/w" 이후 수신자와 메시지를 추출
            sscanf(msg, "/w %s %[^\n]", receiver, message);

            int toSocket = -1;                 // 수신자의 소켓 번호 초기화
            for (int i = 0; i < clnt_cnt; i++) // 모든 클라이언트 확인
            {
                if (strcmp(clients[i].username, receiver) == 0) // 수신자 이름과 일치하는 경우
                {
                    toSocket = i; // 소켓 번호 저장
                    break;        // 루프 종료
                }
            }

            if (toSocket != -1) // 수신자가 존재하는 경우
            {
                char r_message[BUF_SIZE * 2]; // 응답 메시지를 위한 버퍼 크기 증가
                int written_len = snprintf(r_message, sizeof(r_message), "%s%s %s> %s%s\n", GREEN, "귓속말", username, message, END);

                // 버퍼 초과 확인
                if (written_len >= sizeof(r_message))
                {
                    const char *error_msg = "메시지가 너무 깁니다.\n"; // 오류 메시지
                    write(clnt_sock, error_msg, strlen(error_msg));    // 클라이언트에 오류 전송
                }
                else
                {
                    // 수신자에게 메시지 전송
                    write(clients[toSocket].sock, r_message, strlen(r_message));
                }
            }
            else // 수신자가 없는 경우
            {
                const char *error_msg = "해당 사용자가 없습니다.\n"; // 오류 메시지
                write(clnt_sock, error_msg, strlen(error_msg));      // 클라이언트에 오류 전송
            }
        }
        else if (strncmp(msg, "/update_note", 12) == 0) // 메시지가 "/update_note"로 시작하는 경우
        {
            char *action_token = strtok(msg + 12, " "); // "/update_note" 이후 부분을 분리
            if (action_token != NULL)                   // 액션 토큰이 존재하는 경우
            {
                // "read" 토큰을 확인
                char *read_token = strstr(action_token, "read"); // "read"가 포함된 부분 찾기
                if (read_token != NULL)                          // "read"가 존재하는 경우
                {
                    // "read" 다음의 인덱스 부분을 잘라냄
                    read_token += 4;                       // "read"의 길이 4를 더함
                    char *comma = strchr(read_token, 'd'); // "delete_" 이전의 인덱스를 찾기
                    if (comma != NULL)                     // 삭제 토큰이 존재하는 경우
                    {
                        *comma = '\0';                  // ','를 NULL로 바꿔서 문자열 종료
                        char *delete_token = comma + 8; // "delete_" 이후의 부분

                        // Read 인덱스 업데이트
                        process_update(read_token, delete_token); // 업데이트 처리
                    }
                    else
                    {
                        printf("delete_token이 없음\n"); // 삭제 토큰이 없을 경우 메시지 출력
                    }
                }
                else
                {
                    printf("read_token이 NULL이거나 잘못된 형식입니다: '%s'\n", action_token); // 오류 메시지 출력
                }

                printf("끝!\n"); // 끝 메시지 출력
            }
        }
        else // 그 외의 메시지인 경우
        {
            char full_msg[BUF_SIZE];                                                      // 전체 메시지를 저장할 버퍼
            int full_len = snprintf(full_msg, sizeof(full_msg), "%s: %s", username, msg); // 사용자 이름과 메시지를 조합
            send_msg(full_msg, full_len, username);                                       // 메시지 전송 함수 호출
        }
    }
    // 클라이언트 제거 함수 호출
    remove_client(clnt_sock);
    return NULL; // 스레드 종료
}

void remove_client(int clnt_sock)
{
    pthread_mutex_lock(&mutx);         // 뮤텍스 잠금
    for (int i = 0; i < clnt_cnt; i++) // 클라이언트 목록을 순회
    {
        if (clients[i].sock == clnt_sock) // 소켓이 일치하는 경우
        {
            clients[i].loggedIn = 0;               // 로그인 상태 변경
            for (int j = i; j < clnt_cnt - 1; j++) // 클라이언트 목록에서 제거
            {
                clients[j] = clients[j + 1]; // 클라이언트 이동
            }
            clnt_cnt--; // 클라이언트 수 감소
            break;      // 루프 종료
        }
    }
    pthread_mutex_unlock(&mutx); // 뮤텍스 잠금 해제
    close(clnt_sock);            // 소켓 종료
}

void send_msg(const char *msg, int len, const char *sender)
{
    pthread_mutex_lock(&mutx);

    if (sender == "Server") // 전체 공지
    {
        for (int i = 0; i < clnt_cnt; i++)
        {
            write(clients[i].sock, msg, len);
        }
    }
    else // 메시지를 보낸 클라이언트를 제외하고, 같은 채팅방에 있는 사람에게 메시지 전달
    {
        int room = 0;
        for (int i = 0; i < clnt_cnt; i++)
        {
            if (clients[i].username == sender)
            {
                room = clients[i].roomnumber;
            }
        }
        for (int i = 0; i < clnt_cnt; i++)
        {
            if (strcpy(clients[i].username, sender) && clients[i].roomnumber == room)
            {
                write(clients[i].sock, msg, len);
            }
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
    // FILE *file = fopen(USR_FILE, "r, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_CTYPE, ".utf8");
    // FILE *file = fopen(USR_FILE, "r, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_ALL, "ko-KR");                   // 언어 지역 코드를 'ko-KR'(한국어)로 설정한다.
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
    // FILE *file = fopen(USR_FILE, "a, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_CTYPE, ".utf8");
    // FILE *file = fopen(USR_FILE, "a, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_ALL, "ko-KR");                   // 언어 지역 코드를 'ko-KR'(한국어)로 설정한다.

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
    // FILE *file = fopen(USR_FILE, "r, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_CTYPE, ".utf8");
    // FILE *file = fopen(USR_FILE, "r, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_ALL, "ko-KR");                   // 언어 지역 코드를 'ko-KR'(한국어)로 설정한다.

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
        if (strcmp(clients[i].username, username) == 0)
        {
            return clients[i].loggedIn;
            // return 1;
        }
    }
    return 0;
}

/* 등록된 전체 사용자 이름 가져오기 */
int users_read()
{
    FILE *file = fopen(USR_FILE, "r");
    // FILE *file = fopen(USR_FILE, "r, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_CTYPE, ".utf8");
    // FILE *file = fopen(USR_FILE, "r, ccs=UTF-8"); // UTF-8 인코딩으로 파일 열기
    // setlocale(LC_ALL, "ko-KR");                   // 언어 지역 코드를 'ko-KR'(한국어)로 설정한다.
    if (!file)
    {
        perror("fopen() error");
        return 0;
    }

    char file_username[BUF_SIZE];
    char file_password[BUF_SIZE];
    int cnt = 0;

    // 파일에서 사용자 정보를 읽어와 all_users 배열에 저장
    while (fscanf(file, "%s %s", file_username, file_password) != EOF)
    {
        if (cnt < MAX_CLNT)
        {
            strcpy(all_users[cnt], file_username);
            cnt++;
        }
        else
        {
            fprintf(stderr, "Warning: User limit reached. Some users may not be read.\n");
            break;
        }
    }

    fclose(file);
    return cnt; // 실제 읽은 사용자 수 반환
}

/* 쪽지함 기존 정보 저장하기 */
void load_notes()
{
    FILE *file = fopen(USR_NOTE, "r"); // UTF-8 인코딩을 지정하는 옵션을 제거했습니다.
    if (file == NULL)
    {
        perror("파일을 열지 못했습니다.\n");
        return;
    }

    // setlocale(LC_ALL, "ko-KR"); // 언어 지역 코드 설정
    char line[BUF_SIZE * 2]; // 충분한 크기로 임시 버퍼 생성
    note_count = 0;
    while (fgets(line, sizeof(line), file))
    {
        // 디버깅을 위한 출력
        // printf("읽은 줄: %s", line);

        if (sscanf(line, "%d %s %s \"%[^\"]\" %d %d \"%[^\"]\"",
                   &note_list[note_count].index,
                   note_list[note_count].sender,
                   note_list[note_count].receiver,
                   note_list[note_count].content,
                   &note_list[note_count].readYN,
                   &note_list[note_count].deleteYN,
                   note_list[note_count].sendTime) == 7 &&
            note_list[note_count].deleteYN != 1)
        {
            note_count++;
            if (note_count >= NOTE_SIZE)
            {
                fprintf(stderr, "리스트가 찼습니다.\n");
                break;
            }
        }
        else
        {
            fprintf(stderr, "Error parsing line: %s\n", line);
        }
    }

    if (ferror(file))
    {
        perror("파일을 읽지 못했습니다.\n");
    }
    else if (feof(file))
    {
        // 정상 종료
        printf("파일의 끝에 도달했습니다.\n");
    }

    fclose(file);

    // 디버깅을 위한 출력
    // for (int i = 0; i < note_count; i++) {
    //     printf("노트 %d: %d %s %s %s %d %d %s\n",
    //            i, note_list[i].index, note_list[i].sender,
    //            note_list[i].receiver, note_list[i].content,
    //            note_list[i].readYN, note_list[i].deleteYN,
    //            note_list[i].sendTime);
    // }
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

int note_check(const char *username)
{
    int cnt = 0;
    for (int i = 0; i < note_count; i++)
    {
        printf("여기는 서버의 note_check!! %s vs %s\n", note_list[i].receiver, username);
        if (strcmp(note_list[i].receiver, username) == 0)
        {
            user_note_list[i].index = note_list[i].index;
            // printf("Index: %d\n", user_note_list[i].index);
            strcpy(user_note_list[i].sender, note_list[i].sender);
            // printf("Sender: %s\n", user_note_list[i].sender);
            strcpy(user_note_list[i].receiver, note_list[i].receiver);
            // printf("Receiver: %s\n", user_note_list[i].receiver);
            strcpy(user_note_list[i].content, note_list[i].content);
            // printf("Content: %s\n", user_note_list[i].content);
            user_note_list[i].readYN = note_list[i].readYN;
            // printf("ReadYN: %d\n", user_note_list[i].readYN);
            user_note_list[i].deleteYN = note_list[i].deleteYN;
            // printf("DeleteYN: %d\n", user_note_list[i].deleteYN);
            strcpy(user_note_list[i].sendTime, note_list[i].sendTime);
            // printf("SendTime: %s\n", user_note_list[i].sendTime);
            printf("----------------------------\n");
            cnt++;
        }
    }

    printf("여기는 서버의 note_check!! %d\n", cnt);
    return cnt;
}

// 쪽지 정보 텍스트파일 업데이트
void process_update(const char *read, const char *delete)
{
    /* read 문자열을 복사 */
    char *read_cpy = strdup(read);
    /* delete 문자열을 복사, 삭제 데이터가 없을 경우가 있으므로 NULL 체크 및 NULL인 경우를 처리 */
    char *delete_cpy = delete ? strdup(delete) : NULL;

    // readYN 업데이트
    if (read_cpy != NULL) // read_cpy가 NULL이 아닐 경우
    {
        /* read 토큰을 ',' 기준으로 분리하기 */
        char *readToken = strtok(read_cpy, ",");
        while (readToken != NULL) // 분리된 토큰이 존재하는 동안 반복
        {
            /* 문자열을 정수로 변환하여 인덱스를 얻기 */
            int index = atoi(readToken);
            // printf("읽을 인덱스: %d\n", index); // 디버그 출력

            if (index > 0) // 유효한 인덱스인 경우
            {
                /* 노트 목록을 순회하여 인덱스와 일치하는 노트 찾기 */
                for (int j = 0; j < note_count; j++)
                {
                    /* 인덱스가 일치하는 노트의 readYN을 0으로 설정합니다. */
                    if (note_list[j].index == index)
                    {
                        note_list[j].readYN = 0; // 읽은 상태로 업데이트
                        // printf("노트 %d의 readYN을 0으로 설정\n", index); // 디버그 출력
                    }
                }
            }
            // 다음 토큰으로 이동
            readToken = strtok(NULL, ",");
        }
    }
    else // read_cpy가 NULL인 경우
    {
        printf("readYN으로 업데이트 할 데이터가 없습니다.\n"); // 디버그 출력
    }

    /* deleteYN 업데이트 */
    if (delete_cpy != NULL) // delete_cpy가 NULL이 아닐 경우
    {
        /* delete 토큰을 ',' 기준으로 분리 */
        char *deleteToken = strtok(delete_cpy, ",");
        while (deleteToken != NULL) // 분리된 토큰이 존재하는 동안 반복
        {
            // 문자열을 정수로 변환하여 인덱스를 얻습니다.
            int index = atoi(deleteToken);
            // printf("삭제할 인덱스: %d\n", index); // 디버그 출력

            if (index > 0) // 유효한 인덱스인 경우
            {
                /* 노트 목록을 순회하여 인덱스와 일치하는 노트 찾기 */
                for (int j = 0; j < note_count; j++)
                {
                    if (note_list[j].index == index)
                    {
                        note_list[j].deleteYN = 1; // deleteYN을 1로 설정하여 삭제 상태로 변경
                        // printf("노트 %d의 deleteYN을 1로 설정\n", index); // 디버그 출력
                    }
                }
            }
            deleteToken = strtok(NULL, ","); // 다음 토큰으로 이동
        }
    }
    else // delete_cpy가 NULL인 경우
    {
        printf("delete_token이 NULL입니다. 삭제 작업 생략.\n"); // 디버그 출력
    }

    // 업데이트된 노트를 파일에 기록
    FILE *file = fopen(USR_NOTE, "w"); // 노트를 저장할 파일을 쓰기 모드로 엽니다.
    if (file == NULL)                  // 파일 열기 실패 시
    {
        perror("fopen() error"); // 오류 메시지 출력
        free(read_cpy);          // 동적 할당한 메모리 해제
        free(delete_cpy);        // 동적 할당한 메모리 해제
        return;                  // 함수 종료
    }

    /* 노트 정보를 파일에 기록합니다. */
    for (int i = 0; i < note_count; i++)
    {
        fprintf(file, "%d %s %s \"%s\" %d %d \"%s\"\n",
                note_list[i].index,     // 노트 인덱스
                note_list[i].sender,    // 송신자
                note_list[i].receiver,  // 수신자
                note_list[i].content,   // 내용
                note_list[i].readYN,    // 읽음 여부
                note_list[i].deleteYN,  // 삭제 여부
                note_list[i].sendTime); // 전송 시간
    }

    if (fclose(file) != 0) // 파일 닫기 실패 시
    {
        perror("fclose() error"); // 오류 메시지 출력
    }
    else // 파일이 정상적으로 닫혔을 경우
    {
        printf("파일이 성공적으로 작성되었습니다.\n"); // 성공 메시지 출력
    }

    /* 동적 할당한 메모리 해제 */
    free(read_cpy);   // read_cpy 메모리 해제
    free(delete_cpy); // delete_cpy 메모리 해제
}

/* Note 구조체를 파일에 추가하는 함수 */
void add_note_to_file(Note *note)
{
    FILE *file = fopen(USR_NOTE, "a"); // 노트를 추가할 파일을 append 모드로 열기

    /* 파일 열기 실패 시 오류 처리 */
    if (!file)
    {
        perror("fopen() error"); // 오류 메시지 출력
        return;                  // 함수 종료
    }

    /* 노트 정보를 파일에 기록 */
    fprintf(file, "%d %s %s \"%s\" %d %d \"%s\"\n",
            note->index,     // 노트 인덱스
            note->sender,    // 송신자
            note->receiver,  // 수신자
            note->content,   // 내용
            note->readYN,    // 읽음 여부
            note->deleteYN,  // 삭제 여부
            note->sendTime); // 전송 시간

    // 파일 닫기 실패 시 오류 처리
    if (fclose(file) != 0)
    {
        perror("fclose() error"); // 오류 메시지 출력
    }
}

int unoccupiedNum()
{
    int chk = -1;
    for(int i=1; i<11; i++)
    {
        if(chatrms[i].usercount == 0)
        {
            return i;
        }
    }
    
    return chk;
}