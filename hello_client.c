#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr; // 서버 정보를 담는 구조체 선언
	char message[30];
	int str_len;
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);  // 클라이언트 소켓 생성 
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));  // 파일 버퍼에 있는 값 0으로 초기화
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));  // 위 3줄은 내가 접속할 서버의 정보(IP, PORT) 를 담는다.
		
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) // connect():서버와 연결 요청 보내는 곳 
		error_handling("connect() error!");
	
	str_len=read(sock, message, sizeof(message)-1);  // read() : 문자열 읽어오는 함수 (서버 write()를 읽어온다.) 매개변수로 (서버에 새로 만들어진 클라이언트 소켓 , 읽을 문자열, 문자열 크기)
	if(str_len==-1)
		error_handling("read() error!");
	
	printf("Message from server: %s \n", message);  
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}