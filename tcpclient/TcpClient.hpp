#ifndef _TcpClient_hpp
#define _TcpClient_hpp
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
	char data[4096];
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


bool exitApp = false;

class TcpClient {
public:
	SOCKET sock;
	char* host;
	u_short port;
	TcpClient() {};

	~TcpClient() {
		close();
	};
	int init() {
#ifdef _WIN32
		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(version, &data);
#endif
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == sock)
		{
			cout << "server init error" << endl;
			return -1;
		}


	};
	int conn(char* host, u_short port) {
		host = host;
		port = port;
		sockaddr_in _sin;
		_sin.sin_family = AF_INET;

#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(host);
#else
		_sin.sin_addr.s_addr = inet_addr(host);
#endif
		_sin.sin_port = htons(port);

		if (SOCKET_ERROR == connect(sock, (sockaddr*)&_sin, sizeof(_sin)))
		{
			cout << "连接失败" << endl;

			system("pause");
			return -1;
		}

		return 0;

	};
	void onData(void fn(TcpClient* tcpClient)) {

		while (isRun())
		{
			fd_set fd_read;

			FD_ZERO(&fd_read);

			FD_SET(sock, &fd_read);

			timeval t = { 1, 0 };
			
			int selectRes = select(sock + 1, &fd_read, NULL, NULL, &t);

			if (selectRes < 0)
			{
				cout << "select task is end" << endl;
				break;
			}

			if (FD_ISSET(sock, &fd_read))
			{

				FD_CLR(sock, &fd_read);
				/*		UserJoin userJoin;

				if (parseBody(sock, &userJoin)) {
					cout << "???????:" << userJoin.sock << "??????????" << endl;
				}*/

		
				fn(this);

				
			}

		
		}
	};
	void sendData(char* data) {
		int sendRes = send(sock, data, sizeof(data), 0);
		if (SOCKET_ERROR == sendRes) {
			cout << "向" << host  << ":" << port << "发送失败" << endl;
		}
	}
	int recvData(char * data,int len) {
		return recv(sock, data, len, 0);
		

	}
	bool isRun() {
		return SOCKET_ERROR != sock;
	}
	void close() {

#ifdef _WIN32

		WSACleanup();
		closesocket(sock);
#else
		close(sock);
#endif
	};
};
#endif // !_TcpClient_hpp

