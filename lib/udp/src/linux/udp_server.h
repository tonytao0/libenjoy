#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<arpa/inet.h>
#include <netinet/in.h>
#include <string.h>


class UDPServer
{
public:
	UDPServer(std::string ip = "", uint16_t port = 8888);

	~UDPServer();

	void resetIP(std::string ip);

	void resetPort(uint16_t port);

	bool connect();

	bool reconnect(std::string ip, uint16_t port);

	void closeSock();

	void run();

private:
	std::string m_ip;
	uint16_t m_port;
	int m_sockfd;
	struct sockaddr_in m_sock_addr;
};

#endif // !_UDP_SERVER_H_