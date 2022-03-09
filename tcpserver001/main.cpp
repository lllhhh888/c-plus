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

	cout << "���ܵ�����:" << header->cmd << endl;

	cout << "���ݳ���:" <<  header->dataLength << endl;

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
		cout << "��¼username����:" << login.username << endl;
		cout << "��¼password����:" << login.password << endl;
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
		cout <<"�ǳ�username����:" << out.username << endl;
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

	return len <= 0;


}

vector<SOCKET> g_clients;

int main() {
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);

	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		cout << "����socketʧ��" << endl;
		return 0;
	}
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	_sin.sin_port = htons(5555);

	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
		cout << "��ʧ��" << endl;
		return 0;
	}

	listen(_sock, 20);
	cout << "��������ɹ�������5555�˿���" << endl;



	while (true)
	{
		//������ socket
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
		///nfds ��һ������ֵ ��ָfd_set����������������(socket)�ķ�Χ������������
		///���������ļ����������ֵ+1 ��Windows�������������д0
		//timeval t = { 0,0 };
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, NULL);
		if (ret < 0)
		{
			printf("select���������\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			// 4 accept �ȴ����ܿͻ�������
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				printf("����,���ܵ���Ч�ͻ���SOCKET...\n");
			}
			g_clients.push_back(_cSock);
			printf("�¿ͻ��˼��룺socket = %d,IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
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

