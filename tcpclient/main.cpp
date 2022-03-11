#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif

#include <iostream>
#include <thread>

using namespace std;

enum
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_USERJOIN
};

typedef struct _DataHeader
{
	short dataLength;
	short cmd;
} DataHeader;

typedef struct _Login : public _DataHeader
{
	char username[15];
	char password[15];
	_Login()
	{
		cmd = CMD_LOGIN;
		dataLength = sizeof(_Login);
	}
} Login;

typedef struct _DataBody
{
	int code;
	char msg[20];
} DataBody;

typedef struct _LogOut : public _DataHeader
{
	char username[15];
	_LogOut()
	{
		cmd = CMD_LOGOUT;
		dataLength = sizeof(_LogOut);
	}
} LogOut;

typedef struct _UserJoin
{
	int sock;

} UserJoin;

typedef struct _InfoData
{
	int age;
	char name[5];
} InfoData;

template <typename T>
bool parseBody(SOCKET sock, T* body)
{

	char headerChar[50] = {};

	int headerSize = sizeof(DataHeader);

	int recvHeadLen = recv(sock, headerChar, headerSize, 0);

	if (recvHeadLen <= 0)
		return false;

	DataHeader* header = (DataHeader*)headerChar;

	cout << header->cmd << ;
	cout << header->dataLength << endl;

	int bodyLen = recv(sock, (char*)body, header->dataLength - headerSize, 0);

	return bodyLen >= 0;
}

bool threadPause = true;
bool exitApp = false;
Login login;
LogOut logout;
void threadFun(SOCKET client)
{
	char inputMsg[50] = {};

	while (true)
	{
		char username[15];
		char password[15];
		memset(&username, 0, sizeof(username));
		memset(&password, 0, sizeof(password));
		memset(&inputMsg, 0, sizeof(inputMsg));
		threadPause = true;
		cout << "请输入命令： ";
		scanf("%s", inputMsg);

		if (0 == strcmp(inputMsg, "login"))
		{
			cout << "请输入用户名: ";
			scanf("%s", username);
			cout << "请输入用户密码: ";
			scanf("%s", password);
			strcpy(login.username, username);
			strcpy(login.password, password);
			send(client, (char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(inputMsg, "logout"))
		{

			cout << "请输入登出账户:";
			scanf("%s", username);
			strcpy(logout.username, username);
			send(client, (char*)&logout, sizeof(logout), 0);
		}
		else if (0 == strcmp(inputMsg, "exit"))
		{
			// cout << "?????????3??????????" << endl;
			// Sleep(1000);
			// cout << "?????????2??????????" << endl;
			// Sleep(1000);
			// cout << "?????????1??????????" << endl;
			// Sleep(1000);
			// cout << "?????????0??????????" << endl;
			exitApp = true;
		}
		else
		{
			cout << "不存在该命令，请重新输入" << endl;
			continue;
		}
		while (threadPause)
			;
	}
}

int main()
{

#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif

	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == client)
	{
		cout << "server init error" << endl;
		return 0;
	}

	sockaddr_in _sin;
	_sin.sin_family = AF_INET;

#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else
	_sin.sin_addr.s_addr = inet_addr("192.168.3.100");
#endif
	_sin.sin_port = htons(5555);

	if (SOCKET_ERROR == connect(client, (sockaddr*)&_sin, sizeof(_sin)))
	{
		cout << "连接失败" << endl;
		system("pause");
		return 0;
	}

	thread t1(threadFun, client);
	t1.detach();

	while (!exitApp)
	{
		fd_set fd_read;

		FD_ZERO(&fd_read);

		FD_SET(client, &fd_read);

		timeval t = { 1, 0 };

		int selectRes = select(client + 1, &fd_read, NULL, NULL, &t);

		if (selectRes < 0)
		{
			cout << "select task is end" << endl;
			break;
		}

		if (FD_ISSET(client, &fd_read))
		{

			FD_CLR(client, &fd_read);
			/*		UserJoin userJoin;

			if (parseBody(client, &userJoin)) {
				cout << "???????:" << userJoin.sock << "??????????" << endl;
			}*/

			char headerChar[50] = {};
			int headerSize = sizeof(DataHeader);

			int recv_header_len = recv(client, headerChar, headerSize, 0);

			if (recv_header_len <= 0)
			{
				cout << "头部接收错误" << endl;
				continue;
			}

			DataHeader* header = (DataHeader*)headerChar;

			cout << header->cmd << endl;
			cout << header->dataLength << endl;

			int bodyLen;
			DataBody result;

			bodyLen = recv(client, (char*)&result, header->dataLength - headerSize, 0);
			if (bodyLen <= 0)
			{
				cout << endl;
				break;
			}
			cout << "code:" << result.code << endl;
			cout << "msg:" << result.msg << endl;

			switch (header->cmd)
			{
			case CMD_LOGIN:

				if (result.code == 1)
				{
					cout << login.username << "登录成功" << endl;
				}
				else
				{
					cout << login.username << "登录失败" << endl;
				}
				threadPause = false;
				break;
			case CMD_LOGOUT:
				if (result.code == 1)
				{
					cout << "登出成功" << endl;
				}
				else
				{
					cout << "登出失败" << endl;
				}
				threadPause = false;
				break;
			default:
				break;
			}
		}
	}


#ifdef _WIN32

	WSACleanup();
	closesocket(client);
#else
	close(client);
#endif

	return 0;
}