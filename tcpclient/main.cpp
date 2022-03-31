

#include "TcpClient.hpp"
#include <thread>
#include <iostream>

using namespace std;
template <typename T>


bool parseBody(SOCKET sock, T* body)
{

	char headerChar[50] = {};

	int headerSize = sizeof(DataHeader);

	int recvHeadLen = recv(sock, headerChar, headerSize, 0);

	if (recvHeadLen <= 0)
		return false;

	DataHeader* header = (DataHeader*)headerChar;

	cout << header->cmd << endl;
	cout << header->dataLength << endl;

	int bodyLen = recv(sock, (char*)body, header->dataLength - headerSize, 0);

	return bodyLen >= 0;
}



Login login;
LogOut logout;


bool threadPause = true;

void threadFun(TcpClient  client)
{

	while (true) {
		strcpy(login.username, "lh");
		strcpy(login.password, "123456");
		cout << sizeof(login) << endl;
		send(client.sock, (char*)&login, sizeof(login), 0);
	}
	//char inputMsg[50] = {};
	//bool isInput = true;
	//while (isInput)
	//{
	//	char username[15];
	//	char password[15];
	//	memset(&username, 0, sizeof(username));
	//	memset(&password, 0, sizeof(password));
	//	memset(&inputMsg, 0, sizeof(inputMsg));
	//	threadPause = true;
	//	cout << "请输入命令： ";
	//	scanf("%s", inputMsg);

	//	if (0 == strcmp(inputMsg, "login"))
	//	{
	//		cout << "请输入用户名: ";
	//		scanf("%s", username);
	//		cout << "请输入用户密码: ";
	//		scanf("%s", password);
	//		strcpy(login.username, username);
	//		strcpy(login.password, password);
	//		send(client.sock, (char*)&login, sizeof(login), 0);
	//	}
	//	else if (0 == strcmp(inputMsg, "logout"))
	//	{

	//		cout << "请输入登出账户:";
	//		scanf("%s", username);
	//		strcpy(logout.username, username);
	//		send(client.sock, (char*)&logout, sizeof(logout), 0);
	//	}
	//	else if (0 == strcmp(inputMsg, "exit"))
	//	{
	//		client.close();
	//		threadPause = false;
	//		isInput = false;
	//		// cout << "?????????3??????????" << endl;
	//		// Sleep(1000);
	//		// cout << "?????????2??????????" << endl;
	//		// Sleep(1000);
	//		// cout << "?????????1??????????" << endl;
	//		// Sleep(1000);
	//		// cout << "?????????0??????????" << endl;
	//		
	//	}
	//	else
	//	{
	//		cout << "不存在该命令，请重新输入" << endl;
	//		continue;
	//	}
	//	while (threadPause);
	//}
}



void respFn(TcpClient* tcpClient) {


	DataHeader header;
	int dataLen;
	dataLen = tcpClient->recvData((char*)&header, sizeof(header));
	if (dataLen <= 0) return;
	DataBody result;
	dataLen = tcpClient->recvData((char*)&result, sizeof(result));
	if (dataLen <= 0) return;
	cout << "code:" << result.code << endl;
	cout << "msg:" << result.msg << endl;
	switch (header.cmd)
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
		break;
	default:
		break;
	}
	threadPause = false;

}

int main()
{




	TcpClient client;
	client.init();

	thread t1(threadFun, client);
	t1.detach();


	client.conn("127.0.0.1", 5555);
	client.onData(respFn);


	system("pause");




	return 0;
}