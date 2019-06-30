#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "Chatting.h"
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")

using namespace std;
DWORD WINAPI SendThread(LPVOID lpData);

#define BUFFERSIZE 1024
string name;

Chatting Chatting::StaticChatting;

int main()
{
	// 소켓 라이브러리 초기화
	WSADATA wsaData;
	int token = WSAStartup(WINSOCK_VERSION, &wsaData);
	char PORT[5];
	char IP[15];
	// 소켓 생성
	SOCKET socket_client = socket(AF_INET, SOCK_STREAM, 0);

	cout << "사용할 IP 주소 : " << endl;
	cin >> IP;

	cout << "사용할 포트 번호 : " << endl;
	cin >> PORT;

	cout << "이름을 입력하세요. " << endl;
	cin >> name;

	
	// 소켓 주소 정보
	SOCKADDR_IN servAddr = { 0 };
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(IP);
	servAddr.sin_port = htons(atoi(PORT)); // 포트

	// 소켓 접속
	if (connect(socket_client, (struct sockaddr*) & servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		closesocket(socket_client);
		printf("서버에 접속 할수 없습니다.");
		return SOCKET_ERROR;
	}

	DWORD dwTmp;

	CreateThread(NULL, 0, SendThread, (LPVOID)socket_client, 0, NULL);
	// 상태 출력
	Chatting::StaticChatting.DrawWindow();
	Gotoxy(22, 4);
	// 전송 & 수신 루푸
	while (true)
	{
		
		char buffer[1024] = { 0 };
		int len = recv(socket_client, buffer, sizeof(buffer), 0);
		if (len <= 0)
		{
			printf("접속 종료.");
			break;
		}
		// 메시지 출력
		string str = buffer;
		Chatting::StaticChatting.Chat(str);
		Chatting::StaticChatting.DrawWindow();
		Gotoxy(22, 4);
	}
	//소켓 해제
	closesocket(socket_client);
	return 0;
}


DWORD WINAPI SendThread(LPVOID lpData)
{
	SOCKET socket_client = (SOCKET)lpData;
	while (1)
	{
		DWORD dwTmp;
		char text[BUFFERSIZE] = { 0 };
		ReadFile(GetStdHandle(STD_INPUT_HANDLE), text, 1024, &dwTmp, NULL);
		string outputText = name + " : " + text;
		send(socket_client, outputText.c_str(), outputText.length(), 0);
		string str = outputText;
		Chatting::StaticChatting.Chat(str);
		Chatting::StaticChatting.DrawWindow();
		Gotoxy(22, 4);
	}
	
	return 0;
}
