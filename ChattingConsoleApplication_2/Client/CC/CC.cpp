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
#define PORT 5000			// �����Ʈ�� 5000
#define MAX_CLIENT 5		// �ִ� ��� �ο� �� 5��
#define ALLOW 65535			// �ִ� ���� ���� ���� ��ȣ 65535

void recv_client(void* ns);	// ������ �Լ� ������ Ÿ��
int client_num = 0;			// ���� Ƚ�� (Ŭ���̾�Ʈ ����)
int seat = 0;				// Ŭ���Ʈ ��ȣ
char welcome_ok[] = "�� Ŭ���̾�Ʈ, ȯ���մϴ�.\n\0";	// Welcome ���� �ʱ� ��
char welcome_full[] = "����á���ϴ�.\n";				// Welcome ����� �ʰ��� ����� ��
int client_sock[ALLOW];		// client_sock (Ŭ���̾�Ʈ Welcome Socket)
HANDLE hMutex = 0;			// ���ý�

ofstream outFile("LogMessage.txt");

int main()
{
	// Welcome Screen
	printf("+---------------------------+\n");
	printf("+ �����带 �̿��� ��Ƽ ä�� +\n");
	printf("+ ���� �κ�                 +\n");
	printf("+ http://cakel.tistory.com  +\n");
	printf("+ http://ccl.korea.ac.kr    +\n");
	printf("+---------------------------+\n");


	// ���ý� ����
	hMutex = CreateMutex(NULL, FALSE, NULL);	// ���� ���н� ����
	if (!hMutex)
	{
		printf("Mutex ����\n");
		CloseHandle(hMutex);
		return 1;
	}

	// ���� �ʱ�ȭ
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)	// ��� ���� ������ 1.1
	{
		printf("Winsock ����\n");
		WSACleanup();
		return 1;

	}

	// Listen ���� ����
	int s, addrsize, ret;
	sockaddr_in server, client;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == SOCKET_ERROR)
	{
		printf("socket() ����\n");
		closesocket(s);
		WSACleanup();
		return 1;
	}

	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	// Bind �ϱ�
	if (bind(s, (sockaddr*)& server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind() ����\n");
		closesocket(s);
		WSACleanup();
		return 1;

	}

	printf("Ŭ���̾�Ʈ�� ��ٸ��ϴ�. %d ����\n", MAX_CLIENT - client_num);
	listen(s, 10);
	addrsize = sizeof(client);

	// ������� ������ ��ٸ��ϴ�.
	while (1)
	{
		// Blocking ������� Client �� ��ٸ��ϴ�.
		client_sock[seat] = accept(s, (sockaddr*)& client, &addrsize);
		
		// accept ��(�߿�, client_num �� accept() �Լ� ������ �� ���Ҽ� �����Ƿ�
		// MAX_CLIENT ���޽ö� ���� accept() �� ���� �߻� ���ɼ� ����
		if (client_num < MAX_CLIENT)		// ���� ���� �ϱ�
		{

			if (client_sock[seat] != INVALID_SOCKET || client_sock[seat] != SOCKET_ERROR) {}
			_beginthread(recv_client, 0, &client_sock[seat]);
			Sleep(10);
			printf("%d�� Ŭ���̾�Ʈ %s:%d ���� ����\n", seat, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		}

		else	// ���� ã��.
		{
			addrsize = sizeof(client);
			if (client_sock[seat] == INVALID_SOCKET)
			{
				printf("accept() ����\n");
				closesocket(client_sock[seat]);
				closesocket(s);
				WSACleanup();
				return 1;
			}

			ret = send(client_sock[seat], welcome_full, sizeof(welcome_full), 0);
			closesocket(client_sock[seat]);

			// �޽��� ������ �ٷ� ���´�.

		}
	}

	return 0;

}

void recv_client(void* ns)
{
	// ���������� �޾� �帱��, ������ ����
	// Ŭ���̾�Ʈ�� ���ڸ� �ø�
	WaitForSingleObject(hMutex, INFINITE);
	client_num++;				// Ŭ���̾�Ʈ ���� ����
	seat++;						// Ŭ���̾�Ʈ ��ȣ ����
	printf("���� %d�� ����\n", MAX_CLIENT - client_num);		// ������ �Ǵ�

	ReleaseMutex(hMutex);


	char welcome[100] = { 0 };		// accept �� ���Ͽ��� �� ���� ����
	char buff[1024] = { 0 };
	int ret, i;


	itoa(seat, welcome, 10);			// Ŭ���̾�Ʈ ��ȣ
	strcat(welcome, welcome_ok);		// ���� ȯ�� �޽��� ȯ��
	ret = send(*(SOCKET*)ns, welcome, sizeof(welcome), 0);		// ����

	while (ret != SOCKET_ERROR || ret != INVALID_SOCKET)
	{
		ret = recv(*(SOCKET*)ns, buff, 1024, 0);		// Ŭ���̾�Ʈ�� �޽����� ����
		
		SOCKADDR_IN my_addr;
		int len = sizeof(my_addr);
		getsockname(*(SOCKET*)ns, (SOCKADDR*)& my_addr, &len);
		char ipstr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &my_addr.sin_addr, ipstr, sizeof(ipstr));

		// ���� �Է� (����)
		ofstream output("LogMessage.txt", ios::app);
		const time_t t = time(NULL);

		output << ctime(&t) << ipstr<<":"<< my_addr.sin_port << " -> " << buff << endl;

		output.close();
		
		// ���� �б�
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

		// broadcast �κ�
		for (i = 0; i < ALLOW; i++)
		{
			// ���� Ŭ���̾�Ʈ ������ �޸� �ּҿ� ������ Ŭ���̾�Ʈ ���� �޸� �ּҰ� �ٸ����� ����
			WaitForSingleObject(hMutex, INFINITE);
			if (((unsigned*)& client_sock[i] != (SOCKET*)ns))
			{
				send(client_sock[i], buff, strlen(buff), 0);
			}

			ReleaseMutex(hMutex);


		}

		// ���� �ܼ� â�� ���� ��� ����
		if (strlen(buff) != 0) printf("%d �޽��� ���� : %s", strlen(buff), buff);

		memset(buff, 0, 1024);

	}
	if (client_num == 0)
	{
		outFile.close();
	}

	// ���ӵ� ������ ������ ���� ��������
	WaitForSingleObject(hMutex, INFINITE);
	client_num--;
	printf("%d Ŭ���̾�Ʈ ����\n���� %d�� ����\n", seat, MAX_CLIENT - client_num);
	ReleaseMutex(hMutex);

	closesocket(*(int*)ns);

	return;
}
