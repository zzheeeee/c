#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock; // 서버 소켓(ip와 port) 담을 정수값 (중복되지 않음)
	int clnt_sock; // 클라이언트 소켓 값 

	struct sockaddr_in serv_addr;  // 서버소켓을 담을 구조체 선언
	struct sockaddr_in clnt_addr; // 클라이언트 소켓을 담을 구조체 선언 
	socklen_t clnt_addr_size;

	char message[]="Hello World!";
	
	if(argc!=2){     // 메인함수의 매개변수 값이 들어오지 않는다면 실행되는 곳  즉, 소켓이 정상적으로 생성되지 않을 시 실행된다.
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0); // 서버 소켓 생성 
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr)); // memeset (): file 버퍼에 있는 값을 0으로 초기화 시켜줌 
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY); // 서버 주소(ip)를 변환시키는 방법
	serv_addr.sin_port=htons(atoi(argv[1]));    // 서버 포트를 변환 시키는 방법  (소켓에 담기 위해)
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )  // bind() : 송장에 적는다 보내는 주소 
		error_handling("bind() error"); 
	
	if(listen(serv_sock, 5)==-1)                        // listen() : 클라이언트 요청이 들어올 때까지 대기 시켜 놓는 작업
		error_handling("listen() error");
	/////////////////////////////////////////////////////////////////////////////////// 대기상태 (준비단계)
	clnt_addr_size=sizeof(clnt_addr);  
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);  // clnt_sock :  클라이언트 소켓connect()를  accept()로 받아와  서버안에 새로운 클라이언트 소켓 만들어짐 
	if(clnt_sock==-1)    ////connect()를 accept()로 받얐을 시 반환되는 값이 -1이면  실행 (즉, -1이면 함수가 실행되지 않았다는 의미 )
		error_handling("accept() error");  
	
	write(clnt_sock, message, sizeof(message));   // write() : 데이터를 전송하는 곳  (주고 받을 데이터를 write()함수로 공유 ) 매개변수로 (새로 만들어진 클라이언트 소켓, 문자열, 문자열 크기)
	close(clnt_sock);	// 데이터 공유 되면 클라이언트 소켓 종료시킴 
	close(serv_sock);  // 데이터 공유 되면 서버 소켓 종료 시킴
	return 0;
}

void error_handling(char *message) // 소켓이 생성되지 않으면 종료 시키는 함수 
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}