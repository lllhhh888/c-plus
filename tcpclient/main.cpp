#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <thread>

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

typedef struct _UserJoin {
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

	cout << "���յ�����: " << header->cmd << endl;
	cout << "���յ����ݳ��ȣ�" << header->dataLength << endl;

	int bodyLen = recv(sock, (char*)body, header->dataLength - headerSize, 0);

	return bodyLen >= 0;
}


bool threadPause = true;
bool exitApp = false;
Login login;
LogOut logout;
void threadFun(SOCKET client) {
	char inputMsg[50] = {};

	while (true) {
		char username[15];
		char password[15];
		memset(&username, 0, sizeof(username));
		memset(&password, 0, sizeof(password));
		memset(&inputMsg, 0, sizeof(inputMsg));
		threadPause = true;
		cout << "���������";
		scanf("%s", inputMsg);

		if (0 == strcmp(inputMsg, "login")) {
			cout << "�������û���: ";
			scanf("%s", username);
			cout << "����������: ";
			scanf("%s", password);
			strcpy(login.username, username);
			strcpy(login.password, password);
			send(client, (char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(inputMsg, "logout")) {

			cout << "������ǳ����˻���";
			scanf("%s", username);
			strcpy(logout.username, username);
			send(client, (char*)&logout, sizeof(logout), 0);

		    
		}
		else if (0 == strcmp(inputMsg, "exit")) {
			cout << "������3�����˳�" << endl;
			Sleep(1000);
			cout << "������2�����˳�" << endl;
			Sleep(1000);
			cout << "������1�����˳�" << endl;
			Sleep(1000);
			cout << "������0�����˳�" << endl;
			exitApp = true;
		
		}
		else {
			cout << "û�Ҳ���������������������" << endl;
			continue;
		}
		while (threadPause);

	}




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

	thread t1(threadFun, client);
	t1.detach();

	while (!exitApp) {
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
			/*		UserJoin userJoin;

			if (parseBody(client, &userJoin)) {
				cout << "�ͻ���:" << userJoin.sock << "����Ⱥ��" << endl;
			}*/

			char headerChar[50] = {};
			int headerSize = sizeof(DataHeader);
		
			int recv_header_len = recv(client, headerChar, headerSize, 0);

			if (recv_header_len <= 0) {
				cout << "header���մ���" << endl;
				continue;
			}

			DataHeader* header = (DataHeader*)headerChar;


			cout << "���յ�����: " << header->cmd << endl;
			cout << "���յ����ݳ��ȣ�" << header->dataLength << endl;

			int bodyLen;
			DataBody result;

			bodyLen = recv(client, (char*)&result, header->dataLength - headerSize, 0);
			if (bodyLen <= 0) {
				cout << "body���մ���" << endl;
				break;
			}
			cout << "���յ�code������" << result.code << endl;
			cout << "���յ�msg������" << result.msg << endl;


			switch (header->cmd)
			{
			case CMD_LOGIN:
		
				if (result.code == 1) {
					cout << login.username << "��¼�ɹ�" << endl;
				}
				else {
					cout << login.username << "��¼ʧ��" << endl;
				}
				threadPause = false;
				break;
			case CMD_LOGOUT:
				if (result.code == 1) {
					cout <<  "�ǳ��ɹ�" << endl;
				}
				else {
					cout << "�ǳ�ʧ��" << endl;
				}
				threadPause = false;
				break;
			default:
				break;
			}

		}

	}


	WSACleanup();
	closesocket(client);
	return 0;
}