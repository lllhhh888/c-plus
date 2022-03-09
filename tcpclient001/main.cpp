#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

using namespace std;

enum {
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_USERJOIN
};

typedef struct _DataHeader {
	short dataLength;
	short cmd;
}DataHeader;

typedef struct _Login : public _DataHeader {
	char username[15];
	char password[15];
	_Login() {
		cmd = CMD_LOGIN;
		dataLength = sizeof(_Login);
	}
}Login;

typedef struct _DataBody {
	int code;
	char msg[20];
}DataBody;

typedef struct _LogOut : public _DataHeader {
	char username[15];
	_LogOut() {
		cmd = CMD_LOGOUT;
		dataLength = sizeof(_LogOut);
	}
}LogOut;

typedef struct _UserJoin  {
	int sock;

}UserJoin;

typedef struct _InfoData
{
	int age;
	char name[5];
}InfoData;


template <typename T> bool parseBody(SOCKET sock, T* body) {

	char headerChar[50] = {};

	int headerSize = sizeof(DataHeader);

	int recvHeadLen = recv(sock, headerChar, headerSize, 0);

	if (recvHeadLen <= 0) return false;


	DataHeader* header = (DataHeader*)headerChar;

	int bodyLen = recv(sock, (char*)body, header->dataLength - headerSize, 0);

	return bodyLen >= 0;
}


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



	while (true) {
		FD_SET fd_read;

		FD_ZERO(&fd_read);

		FD_SET(client, &fd_read);

		timeval t = { 1,0 };

		int selectRes = select(client, &fd_read, NULL, NULL, &t);

		if (selectRes < 0) {
			cout << "select task is end" << endl;
			break;
		}

		if (FD_ISSET(client, &fd_read)) {
			FD_CLR(client, &fd_read);

			UserJoin userJoin;

			if (parseBody(client, &userJoin)) {
				cout << "�ͻ���:" << userJoin.sock << "����Ⱥ��" << endl;
			}

		}

		cout << "����ʱ������¼����" << endl;

		Login login;
		strcpy(login.username, "lh");
		strcpy(login.password, "123456");
		send(client, (char*)&login, sizeof(login), 0);

		DataBody result;
		if (parseBody(client, &result)) {
			cout << result.code << endl;
			cout << result.msg << endl;
		}
		

	}

	//char cmd[50] = {};
	//while (true)
	//{




	//	char* msg = NULL;
	//	DataBody result;

	//	memset(cmd, 0, sizeof(cmd));

	//	cout << "����������Ҫ���͵���Ϣ: " << endl;

	//	scanf("%s", cmd);

	//	if (0 == strcmp("login", cmd)) {
	//		Login login;

	//		cout << "�������û���" << endl;

	//		scanf("%s", login.username);

	//		cout << "����������" << endl;

	//		scanf("%s", login.password);

	//		send(client, (char*)&login, sizeof(login), 0);


	//		int parseRes = parseBody(client, &result);

	//		if (!parseRes) break;

	//		cout << "���ܵ�����code:" << result.code << ",msg:" << result.msg << ";" << endl;

	//		if (1 == result.code) {
	//			cout << "��¼�ɹ�" << endl;
	//		}
	//		else {
	//			cout << "��¼ʧ��" << endl;
	//		}


	//	}
	//	else if (0 == strcmp("logout", cmd)) {
	//		LogOut  logout;


	//		cout << "������ǳ����˻�" << endl;

	//		scanf("%s", logout.username);

	//		send(client, (char*)&logout, sizeof(logout), 0);

	//		int parseRes = parseBody(client, &result);

	//		if (!parseRes) break;

	//		cout << "���ܵ�����code:" << result.code << ",msg:" << result.msg << ";" << endl;

	//		if (1 == result.code) {
	//			cout << "�ǳ��ɹ�" << endl;
	//		}
	//		else {
	//			cout << "�ǳ�ʧ��" << endl;
	//		}




	//		//dataHeader.cmd = CMD_LOGOUT;
	//	}
	//	else {
	//		cout << "������ڣ�����������" << endl;
	//		continue;
	//	}




	//}









	WSACleanup();
	closesocket(client);
	system("pause");
	return 0;
}