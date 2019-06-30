#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <iostream>

using namespace std;

#define SERVERPORT	9000		//	서버 포트
#define BUFSIZE		512			//	채팅 글자 제한