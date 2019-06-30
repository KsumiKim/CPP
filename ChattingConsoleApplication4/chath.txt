#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;

class Chatting
{
private:
	Chatting();
public:
	void DrawWindow();
	void DrawChattingLog();
	void Chat(string);
	static Chatting StaticChatting;
private:
	string chattingLog[20];
	string userName[10];
	int chattingLogCount;
};

void Gotoxy(int y, int x);