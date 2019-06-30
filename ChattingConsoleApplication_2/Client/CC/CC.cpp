#pragma warning(disable:4996)
#include <stdio.h>
#include <process.h>
#include <Winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>

#include <time.h>
#include <fstream>
#include <iostream>
using namespace std;
#define MAX_SIZE 1000
char inputString[MAX_SIZE];


#pragma comment(lib, "wsock32.lib")	
#define PORT 5000			// 사용포트는 5000
#define MAX_CLIENT 5		// 최대 허용 인원 수 5개
#define ALLOW 65535			// 최대 생성 가능 소켓 번호 65535

void recv_client(void* ns);	// 스레드 함수 프로토 타입
int client_num = 0;			// 점유 횟수 (클라이언트 갯수)
int seat = 0;				// 클라언트 번호
char welcome_ok[] = "번 클라이언트, 환영합니다.\n\0";	// Welcome 정상 초기 글
char welcome_full[] = "가득찼습니다.\n";				// Welcome 사용자 초가시 생기는 글
int client_sock[ALLOW];		// client_sock (클라이언트 Welcome Socket)
HANDLE hMutex = 0;			// 뮤택스

ofstream outFile("LogMessage.txt");

int main()
{
	// Welcome Screen
	printf("+---------------------------+\n");
	printf("+ 스레드를 이용한 멀티 채팅 +\n");
	printf("+ 서버 부분                 +\n");
	printf("+ http://cakel.tistory.com  +\n");
	printf("+ http://ccl.korea.ac.kr    +\n");
	printf("+---------------------------+\n");


	// 뮤택스 생성
	hMutex = CreateMutex(NULL, FALSE, NULL);	// 생성 실패시 오류
	if (!hMutex)
	{
		printf("Mutex 오류\n");
		CloseHandle(hMutex);
		return 1;
	}

	// 윈속 초기화
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)	// 사용 소켓 버전은 1.1
	{
		printf("Winsock 오류\n");
		WSACleanup();
		return 1;

	}

	// Listen 소켓 생성
	int s, addrsize, ret;
	sockaddr_in server, client;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == SOCKET_ERROR)
	{
		printf("socket() 오류\n");
		closesocket(s);
		WSACleanup();
		return 1;
	}

	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	// Bind 하기
	if (bind(s, (sockaddr*)& server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind() 오류\n");
		closesocket(s);
		WSACleanup();
		return 1;

	}

	printf("클라이언트를 기다립니다. %d 남음\n", MAX_CLIENT - client_num);
	listen(s, 10);
	addrsize = sizeof(client);

	// 사용자의 접속을 기다립니다.
	while (1)
	{
		// Blocking 방식으로 Client 를 기다립니다.
		client_sock[seat] = accept(s, (sockaddr*)& client, &addrsize);
		
		// accept 시(중요, client_num 가 accept() 함수 수행중 에 변할수 있으므로
		// MAX_CLIENT 도달시랑 따로 accept() 시 문제 발생 가능성 있음
		if (client_num < MAX_CLIENT)		// 정상 맞이 하기
		{

			if (client_sock[seat] != INVALID_SOCKET || client_sock[seat] != SOCKET_ERROR) {}
			_beginthread(recv_client, 0, &client_sock[seat]);
			Sleep(10);
			printf("%d번 클라이언트 %s:%d 에서 접속\n", seat, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		}

		else	// 가득 찾다.
		{
			addrsize = sizeof(client);
			if (client_sock[seat] == INVALID_SOCKET)
			{
				printf("accept() 오류\n");
				closesocket(client_sock[seat]);
				closesocket(s);
				WSACleanup();
				return 1;
			}

			ret = send(client_sock[seat], welcome_full, sizeof(welcome_full), 0);
			closesocket(client_sock[seat]);

			// 메시지 보내고 바로 끊는다.

		}
	}

	return 0;

}

void recv_client(void* ns)
{
	// 정상적으로 받아 드릴때, 스레드 실행
	// 클라이언트의 숫자를 늘림
	WaitForSingleObject(hMutex, INFINITE);
	client_num++;				// 클라이언트 갯수 증가
	seat++;						// 클라이언트 번호 증가
	printf("슬롯 %d개 남음\n", MAX_CLIENT - client_num);		// 갯수로 판단

	ReleaseMutex(hMutex);


	char welcome[100] = { 0 };		// accept 된 소켓에게 줄 버퍼 생성
	char buff[1024] = { 0 };
	int ret, i;


	itoa(seat, welcome, 10);			// 클라이언트 번호
	strcat(welcome, welcome_ok);		// 정상 환영 메시지 환영
	ret = send(*(SOCKET*)ns, welcome, sizeof(welcome), 0);		// 전송

	while (ret != SOCKET_ERROR || ret != INVALID_SOCKET)
	{
		ret = recv(*(SOCKET*)ns, buff, 1024, 0);		// 클라이언트의 메시지를 받음
		
		SOCKADDR_IN my_addr;
		int len = sizeof(my_addr);
		getsockname(*(SOCKET*)ns, (SOCKADDR*)& my_addr, &len);
		char ipstr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &my_addr.sin_addr, ipstr, sizeof(ipstr));

		// 파일 입력 (쓰기)
		ofstream output("LogMessage.txt", ios::app);
		const time_t t = time(NULL);

		output << ctime(&t) << ipstr<<":"<< my_addr.sin_port << " -> " << buff << endl;

		output.close();
		
		// 파일 읽기
		//ifstream inFile("LogMessage.txt");
		//
		//while (!inFile.eof()) {
		//
		//	inFile.getline(inputString, 100);
		//
		//	cout << inputString << endl;
		//
		//}
		//inFile.close();

		// broadcast 부분
		for (i = 0; i < ALLOW; i++)
		{
			// 받은 클라이언트 소켓의 메모리 주소와 보내는 클라이언트 소켓 메모리 주소가 다를때만 전송
			WaitForSingleObject(hMutex, INFINITE);
			if (((unsigned*)& client_sock[i] != (SOCKET*)ns))
			{
				send(client_sock[i], buff, strlen(buff), 0);
			}

			ReleaseMutex(hMutex);


		}

		// 서버 콘솔 창에 전송 기록 남김
		if (strlen(buff) != 0) printf("%d 메시지 보냄 : %s", strlen(buff), buff);

		memset(buff, 0, 1024);

	}
	if (client_num == 0)
	{
		outFile.close();
	}

	// 접속된 소켓이 연결을 해제 시켰을때
	WaitForSingleObject(hMutex, INFINITE);
	client_num--;
	printf("%d 클라이언트 해제\n슬롯 %d개 남음\n", seat, MAX_CLIENT - client_num);
	ReleaseMutex(hMutex);

	closesocket(*(int*)ns);

	return;
}
