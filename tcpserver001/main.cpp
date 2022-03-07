#include <WinSock2.h>
#include <Windows.h>
#include <cstdlib>
#include <iostream>

using namespace std;

int main() {
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);

	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == server) {
		cout << "����socketʧ��" << endl;
		return 0;
	}
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	_sin.sin_port = htons(5555);

	if (SOCKET_ERROR == bind(server, (sockaddr*)&_sin, sizeof(_sin))) {
		cout << "��ʧ��" << endl;
		return 0;
	}

	listen(server, 20);
	cout << "��������ɹ�������5555�˿���" << endl;

	char msg[50] = "hello client";
	int msgLen = strlen(msg);
	while (true)
	{
		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		SOCKET client = accept(server, (sockaddr*)&clientAddr, &clientAddrSize);
		if (INVALID_SOCKET == client) {
			cout << "����ʧ��" << endl;
			continue;
		}
		
		int sendRes = send(client, msg, msgLen, 0);

		if (SOCKET_ERROR == sendRes) {
			cout << "����ʧ��" << endl;
		}
		else {
			cout << "���ͳɹ�" << endl;
		}

	}



	WSACleanup();
	closesocket(server);
	system("pause");
	return 0;
}

