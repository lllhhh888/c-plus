#include <WinSock2.h>
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <vector>

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


bool processTasks(SOCKET client) {

	char headerChar[1024] = {};
	int headerSize = sizeof(DataHeader);

	int recvLen = recv(client, headerChar, headerSize, 0);

	DataHeader* header = (DataHeader*)headerChar;

	cout << "接受到命令:" << header->cmd << endl;

	cout << "数据长度:" <<  header->dataLength << endl;

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
		cout << "登录username参数:" << login.username << endl;
		cout << "登录password参数:" << login.password << endl;
		if (0 == strcmp(login.username, "lh") && 0 == strcmp(login.password, "123456")) {
			result.code = 1;
			strcpy(result.msg, "成功");
		}
		else {
			result.code = 0;
			strcpy(result.msg, "失败");
		}
		send(client, (char*)&result, sizeof(result), 0);
		break;
	case CMD_LOGOUT:
		len = recv(client, (char*)&out, header->dataLength - headerSize, 0);
		if (len <= 0) break;
		cout <<"登出username参数:" << out.username << endl;
		if (0 == strcmp(out.username, "lh")) {
			log_out_result.code = 1;
			strcpy(log_out_result.msg, "登出成功");
		}
		else {
			log_out_result.code = 0;
			strcpy(log_out_result.msg, "没有该账户");
		}
		send(client, (char*)&log_out_result, sizeof(log_out_result), 0);
		break;

	default:
		cout << "进来1" << endl;
		break;
	}

	return len <= 0;


}

vector<SOCKET> g_clients;

int main() {
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);

	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		cout << "创建socket失败" << endl;
		return 0;
	}
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	_sin.sin_port = htons(5555);

	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
		cout << "绑定失败" << endl;
		return 0;
	}

	listen(_sock, 20);
	cout << "启动服务成功，监听5555端口中" << endl;



	while (true)
	{
		//伯克利 socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}
		///nfds 是一个整数值 是指fd_set集合中所有描述符(socket)的范围，而不是数量
		///既是所有文件描述符最大值+1 在Windows中这个参数可以写0
		//timeval t = { 0,0 };
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, NULL);
		if (ret < 0)
		{
			printf("select任务结束。\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			// 4 accept 等待接受客户端连接
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				printf("错误,接受到无效客户端SOCKET...\n");
			}
			g_clients.push_back(_cSock);
			printf("新客户端加入：socket = %d,IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
		}

		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (processTasks(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
					

				}
			}
		}
	}


	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}



	WSACleanup();
	closesocket(_sock);
	system("pause");
	return 0;
}

