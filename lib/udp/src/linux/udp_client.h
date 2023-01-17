#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>

class UDPClient
{
public:
    UDPClient(std::string ip = "127.0.0.1",uint16_t port = 8888);

    ~UDPClient();

    void resetIP(std::string ip);

	void resetPort(uint16_t port);

	bool connect();

	void closeSock();

	void send(const std::string& msg);

	void send(const char*, size_t len);

	uint32_t recv(char* buf, uint32_t len);

private:
    std::string m_ip;
    uint16_t m_port;
    int m_sockfd;
	struct sockaddr_in m_sock_addr;
};

#endif