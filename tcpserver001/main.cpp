#include <WinSock2.h>
#include <Windows.h>
#include <cstdlib>
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

   
	char recvMsg[50] = {};
	int sendRes;
	while (true)
	{
		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		SOCKET client = accept(server, (sockaddr*)&clientAddr, &clientAddrSize);
		if (INVALID_SOCKET == client) {
			cout << "����ʧ��" << endl;
			continue;
		}

		while (true) {
			memset(recvMsg, 0, sizeof(recvMsg));
			InfoData infoData;
			int ret = recv(client, recvMsg, sizeof(recvMsg), 0);
			if (ret <= 0) break;

			cout << "���յ�����" << recvMsg << endl;
			

			if (strcmp("getInfo", recvMsg) == 0) {
				infoData.age = 18;
				strcpy(infoData.name, "lh");
				
				
				sendRes = send(client,(char*)&infoData, sizeof(infoData), 0);
			}
			else if (strcmp("getNum", recvMsg) == 0) {
			    
				sendRes = send(client, "222", 3, 0);
			}
			else {
			
				sendRes = send(client, "???", 3, 0);
			}

			if (SOCKET_ERROR == sendRes) {
				cout << "����ʧ��" << endl;
			}
			else {
				cout << "���ͳɹ�" << endl;
			}
		}
		cout << "end" << endl;

	}



	WSACleanup();
	closesocket(server);
	system("pause");
	return 0;
}

