#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

using namespace std;

typedef struct _InfoData
{
	int age;
	char name[5];
}InfoData;

int main() {
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);


	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == client) {
		cout << "����sockʧ��" << endl;
		return 0;
	}

	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_sin.sin_port = htons(5555);

	if (SOCKET_ERROR == connect(client, (SOCKADDR*)&_sin, sizeof(_sin))) {
		cout << "����ʧ��,���˳�����" << endl;
		system("pause");
		return 0;
	}

	char sendMsg[50] = {};
	while (true)
	{

		InfoData infoData;

		memset(sendMsg, 0, sizeof(sendMsg));
		cout << "����������Ҫ���͵���Ϣ: " << endl;

		scanf("%s", sendMsg);


		if (SOCKET_ERROR == send(client, sendMsg, strlen(sendMsg) + 1, 0)) {
			cout << "����ʧ��" << endl;
			continue;
		}


		if (recv(client, (char*)&infoData, sizeof(infoData), 0) > 0) {
			
			cout << infoData.age << endl;
			cout << infoData.name << endl;
		}
		else {
			cout << "����ʧ��" << endl;
		}
	}









	WSACleanup();
	closesocket(client);
	system("pause");
	return 0;
}