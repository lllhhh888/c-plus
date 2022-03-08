#include <WinSock2.h>
#include <Windows.h>
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

typedef struct _Login : public _DataHeader {
	char username[15];
	char password[15];
	_Login() {
		cmd = CMD_LOGIN;
		dataLength = sizeof(_Login);
	}
}Login;

typedef struct _DataBody {
	short code;
	char msg[20];
}DataBody;

typedef struct _LogOut : public _DataHeader {
	char username[15];
	_LogOut() {
		cmd = CMD_LOGOUT;
		dataLength = sizeof(_LogOut);
	}
}LogOut;

typedef struct _InfoData
{
	int age;
	char name[5];
}InfoData;


template <typename T> bool parseBody (SOCKET sock, T *body) {

	char headerChar[50] = {};

	int headerSize = sizeof(DataHeader);

	int recvHeadLen = recv(sock, headerChar, headerSize, 0);

	if (recvHeadLen <= 0) return false;

	DataHeader*  header = (DataHeader*)headerChar;

	int bodyLen = recv(sock, (char*)body, header->dataLength - headerSize, 0);

	if (bodyLen <= 0) return false;

	return true;
}


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

	char cmd[50] = {};
	while (true)
	{


		char* msg = NULL;
		DataBody result;

		memset(cmd, 0, sizeof(cmd));

		cout << "请输入你需要发送的信息: " << endl;

		scanf("%s", cmd);

		if (0 == strcmp("login", cmd)) {
			Login login;

			cout << "请输入用户名" << endl;

			scanf("%s", login.username);

			cout << "请输入密码" << endl;

			scanf("%s", login.password);

			send(client, (char*)&login, sizeof(login), 0);


			int parseRes = parseBody(client, &result);

			if (!parseRes) break;

			cout << "接受到参数code:" << result.code << ",msg:" << result.msg << ";" << endl;

			if (1 == result.code) {
				cout << "登录成功" << endl;
			}
			else {
				cout << "登录失败" << endl;
			}


		}
		else if (0 == strcmp("logout", cmd)) {
			LogOut  logout;


			cout << "请输入登出的账户" << endl;

			scanf("%s", logout.username);

			send(client, (char*)&logout, sizeof(logout), 0);

			int parseRes = parseBody(client, &result);

			if (!parseRes) break;

			cout << "接受到参数code:" << result.code << ",msg:" << result.msg << ";" << endl;

			if (1 == result.code) {
				cout << "登出成功" << endl;
			}
			else {
				cout << "登出失败" << endl;
			}




			//dataHeader.cmd = CMD_LOGOUT;
		}
		else {
			cout << "命令不存在，请重新输入" << endl;
			continue;
		}


		/*send(client, (char*)&dataHeader, sizeof(dataHeader), 0);
		send(client, msg, dataHeader.dataLength + 1, 0);*/
		//if (SOCKET_ERROR == send(client, cmd, strlen(cmd) + 1, 0)) {
		//	cout << "发送失败" << endl;
		//	continue;
		//}


		//if (recv(client, (char*)&infoData, sizeof(infoData), 0) > 0) {

		//	cout << infoData.age << endl;
		//	cout << infoData.name << endl;
		//}
		//else {
		//	cout << "接受失败" << endl;
		//}

	}









	WSACleanup();
	closesocket(client);
	system("pause");
	return 0;
}