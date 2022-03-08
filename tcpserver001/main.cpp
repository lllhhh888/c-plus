#include <WinSock2.h>
#include <Windows.h>
#include <cstdlib>
#include <iostream>

using namespace std;



enum {
	CMD_LOGIN,
	CMD_LOGOUT
};

typedef struct _DataHeader {
	short dataLength;
	short cmd;
}DataHeader;

typedef struct _Login {
	char username[15];
	char password[15];
}Login;

typedef struct _LogOut {
	char username[15];
}LogOut;

typedef struct _LoginResult : public DataHeader {
	short code;
	char msg[20];
	_LoginResult() {
		cmd = CMD_LOGIN;
		dataLength = sizeof(_LoginResult);
	}
}LoginResult;


typedef struct _DataBoby {
	short code;
	char msg[20];
}DataBody;



typedef struct _LogOutResult : public DataHeader, public _DataBoby {
	_LogOutResult() {
		cmd = CMD_LOGIN;
		dataLength = sizeof(_LoginResult);
	}
}LogOutResult;



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


		while (true)
		{


			char headerChar[1024] = {};
			int headerSize = sizeof(DataHeader);

			int recvLen = recv(client, headerChar, headerSize, 0);

			DataHeader* header = (DataHeader*)headerChar;

			cout << header->cmd << endl;

			cout << header->dataLength << endl;

			int len = 1;
			Login login;
			LogOut out;
			LoginResult result;
			LogOutResult log_out_result;
			switch (header->cmd)
			{

			case CMD_LOGIN:
				len = recv(client, (char*)&login, header->dataLength - headerSize, 0);
				if (len <= 0) break;
				cout << login.username << endl;
				cout << login.password << endl;
				if (0 == strcmp(login.username, "lh") && 0 == strcmp(login.password, "123456")) {
					result.code = 1;
					strcpy(result.msg, "�ɹ�");
				}
				else {
					result.code = 0;
					strcpy(result.msg, "ʧ��");
				}
				send(client, (char*)&result, sizeof(result), 0);
				break;
			case CMD_LOGOUT:
				len = recv(client, (char*)&out, header->dataLength - headerSize, 0);
				if (len <= 0) break;
				cout << out.username << endl;
				if (0 == strcmp(out.username, "lh")) {
					log_out_result.code = 1;
					strcpy(log_out_result.msg, "�ǳ��ɹ�");
				}
				else {
					log_out_result.code = 0;
					strcpy(log_out_result.msg, "û�и��˻�");
				}
				send(client, (char*)&log_out_result, sizeof(log_out_result), 0);
				break;

			default:
				cout << "����1" << endl;
				break;
			}


			

			if (len <= 0) break;
		}

		cout << "�����Ự" << endl;

		//switch (dataHeader.cmd)
		//{
		//CMD_LOGIN:
		//	break;
		//CMD_LOGOUT:
		//	break;
		//default:
		//	break;
		//}





		/*	if (SOCKET_ERROR == sendRes) {
				cout << "����ʧ��" << endl;
			}
			else {
				cout << "���ͳɹ�" << endl;
			}*/

	}



	WSACleanup();
	closesocket(server);
	system("pause");
	return 0;
}

