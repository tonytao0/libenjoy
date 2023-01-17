#include "udp_server.h"
#pragma warning(disable : 4996)


UDPServer::UDPServer(std::string ip, uint16_t port)
	: m_ip(ip)
	, m_port(port)
	, m_sockfd(INVALID_SOCKET)
	, m_sock_addr{ 0 }
{
	//default ip=INADDR_ANY, port=8888.
	memset(&m_sock_addr, 0, sizeof(sockaddr_in));
	m_sock_addr.sin_family = AF_INET;
	m_sock_addr.sin_port = htons(m_port);
	m_sock_addr.sin_addr.S_un.S_addr = INADDR_ANY;
}

UDPServer::~UDPServer()
{
}

void UDPServer::resetIP(std::string ip)
{
	m_ip = ip;
}

void UDPServer::resetPort(uint16_t port)
{
	m_port = port;
}

bool UDPServer::connect()
{
	if (INVALID_SOCKET != m_sockfd)
	{
		close();
	}

	//create socket.
	m_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sockfd == INVALID_SOCKET)
	{
		std::cout << "connect failed.\n";

		return false;
	}

	//config ip and port.
	memset(&m_sock_addr, 0, sizeof(m_sock_addr));
	m_sock_addr.sin_family = AF_INET;
	m_sock_addr.sin_port = htons(m_port);
	if (m_ip.empty())
	{
		m_sock_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		m_sock_addr.sin_addr.S_un.S_addr = inet_addr(m_ip.c_str());
	}
	

	if (bind(m_sockfd, (sockaddr*)&m_sock_addr, sizeof(m_sock_addr)) == SOCKET_ERROR)
	{
		printf("bind error !");

		close();

		return false;
	}

	return true;
}

bool UDPServer::reconnect(std::string ip, uint16_t port)
{
	resetIP(ip);
	resetPort(port);
	return connect();
}

void UDPServer::close()
{
	if (INVALID_SOCKET != m_sockfd)
	{
		closesocket(m_sockfd);

		m_sockfd = INVALID_SOCKET;
	}
}

void UDPServer::run()
{
	sockaddr_in remote_sock_addr;
	int addr_len = sizeof(remote_sock_addr);

	char recvData[255] = {0};
	const char* sendData = "a udp packet from udp server\n";

	while (true)
	{
		int ret = recvfrom(m_sockfd, recvData, 255, 0, (sockaddr*)&remote_sock_addr, &addr_len);
		if (ret > 0)
		{
			recvData[ret] = '\0';
			printf("recive a new connect��%s \r\n", inet_ntoa(remote_sock_addr.sin_addr));
			printf(recvData);
		}

		sendto(m_sockfd, sendData, strlen(sendData), 0, (sockaddr*)&remote_sock_addr, addr_len);
	}
}