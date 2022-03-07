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
		cout << "连接失败,请退出重新" << endl;
		system("pause");
		return 0;
	}

	char sendMsg[50] = {};
	char recvMsg[50] = {};
	int recvMsgSize = sizeof(recvMsg);
	while (true)
	{

	

		memset(sendMsg, 0, sizeof(sendMsg));
		memset(recvMsg, 0, sizeof(recvMsg));
		cout << "请输入你需要发送的信息: " << endl;

		scanf("%s", sendMsg);


		if (SOCKET_ERROR == send(client, sendMsg, strlen(sendMsg) + 1, 0)) {
			cout << "发送失败" << endl;
			continue;
		}


		if (recv(client, recvMsg, recvMsgSize, 0) > 0) {
			cout << recvMsg << endl;
		}
		else {
			cout << "接受失败" << endl;
		}
	}









	WSACleanup();
	closesocket(client);
	system("pause");
	return 0;
}