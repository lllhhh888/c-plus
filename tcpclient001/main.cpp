#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

using namespace std;

int main() {
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);


	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == client) {
		cout << "创建sock失败" << endl;
		return 0;
	}

	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_sin.sin_port = htons(5555);

	if (SOCKET_ERROR == connect(client, (SOCKADDR*)&_sin, sizeof(_sin))) {
		cout << "连接失败" << endl;
		return 0;
	}
	char msg[50] = {};
	int msgLen = sizeof(msg);
	cout << msgLen << endl;
	if (recv(client, msg, msgLen, 0) > 0) {
		cout << msg << endl;
	}
	else {
		cout << "接受失败" << endl;
	}



	WSACleanup();
	closesocket(client);
	system("pause");
	return 0;
}