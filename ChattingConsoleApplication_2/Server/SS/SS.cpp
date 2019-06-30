#pragma warning(disable:4996)
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <process.h>
#pragma comment(lib,"wsock32.lib")

#define PORT 5000			// ����� ��Ʈ�� 5000
#define IP "10.10.14.62"		// ������ ������ ���� ȣ��Ʈ

void recv_thread(void*);	// ������ ���� �Լ��� ������ Ÿ��

int ret = 0;				// ���� ��
int s = 0;					// ���� ��

HANDLE hMutex;				// ���ؽ���

int main()
{
	// Welcome Screen
	printf("+---------------------------+\n");
	printf("+ �����带 �̿��� ��Ƽ ä�� +\n");
	printf("+ Ŭ���̾�Ʈ �κ�           +\n");
	printf("+ http://cakel.tistory.com  +\n");
	printf("+ http://ccl.korea.ac.kr    +\n");
	printf("+---------------------------+\n");

	// ���ý� �ʱ�ȭ
	hMutex = CreateMutex(NULL, FALSE, FALSE);
	if (!hMutex)

	{
		printf("Mutex ���� ����\n");
		return 1;

	}

	// ���� �ʱ�ȭ
	WSADATA wsd;
	char buff[1024];
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("Winsock ����\n");
		return 1;
	}

	// ������ �����ϱ� ���� ���� ����
	sockaddr_in server;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == SOCKET_ERROR)
	{
		printf("socket() ����\n");
		closesocket(s);
		WSACleanup();
		return 1;

	}

	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	// ������ ����
	if (connect(s, (sockaddr*)& server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("connect() ����\n");
		closesocket(s);
		WSACleanup();
		return 1;

	}

	printf("������ ������ �Ǿ���.\n");

	// �������� �켱 �ִ� Welcome �޽��� �м�
	ret = recv(s, buff, 1024, 0);
	printf("%s", buff);

	// ���� á�ٰ� �޽����� �Դٸ�
	if (!strcmp("����á���ϴ�.\n", buff))
	{
		closesocket(s);
		WSACleanup();
		return 0;			// ������ ����
	}

	// ���� ������ �Ǹ� ������ �۵� - �޴� �޽����� ������� �ǽð� ����
	_beginthread(recv_thread, 0, NULL);

	// ������ �޽����� ������� ���� �ʿ䰡 �����ϴ�.
	while (ret != INVALID_SOCKET || ret != SOCKET_ERROR)
	{

		// ������ : CPU ������ ���ҿ�
		Sleep(10);
		printf("���� �޽��� �Է� : ");
		fgets(buff, 1024, stdin);

		// ���� ��� �߸� �� ����� ������� Ż��
		if (ret == INVALID_SOCKET || ret == SOCKET_ERROR) break;

		// ������ ������ ���� fgets �� �ް� ���� ���������� ����
		ret = send(s, buff, strlen(buff), 0);

		// ���� �ʱ�ȭ
		memset(buff, 0, 1024);

	}

	// ����/���������� ������ ��� ������� Ʋ�� ���� �޾��� ��
	printf("������ ������ ������ϴ�.\n");
	closesocket(s);
	WSACleanup();

	return 0;
}

// �޴� ������ �κ�
void recv_thread(void* pData)
{
	int ret_thread = 65535;
	char buff_thread[1024] = { 0 };

	// ������� ���� ���� ����ϴ� ���� �ƴϸ� �޴� �߿� ������ ����� ����ٰ� ���� ����
	while (ret_thread != INVALID_SOCKET || ret_thread != SOCKET_ERROR)
	{
		Sleep(10);	// CPU ������ 100% ������

		// �������� �ִ� �޽����� �ǽð����� ��ٷȴٰ� �޽��ϴ�.
		ret_thread = recv(s, buff_thread, sizeof(buff_thread), 0);

		// �������� �޴� �۾��� �� ��� �������� �� Ż��
		if (ret_thread == INVALID_SOCKET || ret_thread == SOCKET_ERROR)
			break;

		// ���������� ���� ���۸� ���
		printf("\n%d �޽��� ���� : %s", strlen(buff_thread), buff_thread);
		memset(buff_thread, 0, 1024);	// ���� ���۸� �ʱ��
	}

	// �۾��� ���� ������ ��ȿȭ��Ŵ
	WaitForSingleObject(hMutex, 100L);
	ret = INVALID_SOCKET;
	ReleaseMutex(hMutex);

	return;
}