#include "udp_client.h"


UDPClient::UDPClient(std::string ip, uint16_t port)
	: m_ip(ip)
	, m_port(port)
	, m_sockfd(-1)
	, m_sock_addr{0}
{
	//default ip="127.0.0..1", port=8888.
	memset(&m_sock_addr, 0, sizeof(sockaddr_in));
    m_sock_addr.sin_family = AF_INET;
    m_sock_addr.sin_port = htons(m_port);  
    // m_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    m_sock_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
}

UDPClient::~UDPClient()
{
}

void UDPClient::resetIP(std::string ip)
{
	m_ip = ip;
}

void UDPClient::resetPort(uint16_t port)
{
	m_port = port;
}

bool UDPClient::connect()
{
	//create socket.
	m_sockfd =  socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sockfd < 0)
	{
		std::cout << "connect failed.\n";

		return false;
	}

	//config ip and port.
	memset(&m_sock_addr, 0, sizeof(sockaddr_in));
    m_sock_addr.sin_family = AF_INET;
    m_sock_addr.sin_port = htons(m_port);  
    // m_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_sock_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());

    return true;
}

void UDPClient::closeSock()
{
	if (-1 != m_sockfd)
	{
		close(m_sockfd);

		m_sockfd = -1;
	}
}

void UDPClient::send(const std::string &msg)
{
	sendto(m_sockfd, msg.c_str(), msg.length(), 0, (struct sockaddr*)&m_sock_addr, sizeof(m_sock_addr));
}

void UDPClient::send(const char* msg, size_t len)
{
	sendto(m_sockfd, msg, len, 0, (struct sockaddr*)&m_sock_addr, sizeof(m_sock_addr));
}

uint32_t UDPClient::recv(char* buf, uint32_t len)
{
	socklen_t addr_len = sizeof(m_sock_addr);

	return recvfrom(m_sockfd, buf, len, 0, (sockaddr*)&m_sock_addr, &addr_len);
}
