#include <iostream>
#include <WinSock2.h>

// 添加动态库的lib
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"Winmm.lib")

using namespace std;

int main()
{
	// 初始化socket环境
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0)
	{
		cout << "WSAStartup Error:" << WSAGetLastError() << endl;
		return 0;
	}

	// 1. 创建数据报sock
	SOCKET c = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (c == INVALID_SOCKET)
	{
		cout << "socket error:" << WSAGetLastError() << endl;
		return 0;
	}

	// 服务端的地址
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8000); //网络字节序
	addr.sin_family = AF_INET;

	int len = sizeof(sockaddr_in);
	int ret = 0;
	do
	{
		// 2. 可以sendto发送和recvfrom接收了
		ret = sendto(c, "I am UDP Client!", strlen("I am UDP Client!"), 0, (SOCKADDR*)&addr, len);

		char buf[100] = { 0 };
		recvfrom(c, buf, 100, 0, NULL, NULL); //不关注服务端地址

		cout << "recvfrom " << ret << ":" << buf << endl;

		Sleep(1000);
	} while (ret != SOCKET_ERROR && ret != 0);

	// 3.关闭套接字
	closesocket(c);

	// 清理winsock2环境
	WSACleanup();

	return 0;
}
