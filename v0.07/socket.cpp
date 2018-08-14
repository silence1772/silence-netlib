#include "socket.h"
#include <string.h>
#include <netinet/in.h>
#include "inetaddress.h"
#include "socketops.h"

Socket::~Socket()
{
	Close(sockfd_);
}

void Socket::Bind(const InetAddress& addr)
{
	Bind(sockfd_, addr.GetSockAddrInet());
}

void Socket::Listen()
{
	Listen(sockfd_);
}

int Socket::Accept(InetAddress* peer_addr)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	int connfd = Accept(sockfd_, &addr);
	if (connfd >= 0)
	{
		peer_addr->SetSockAddrInet(addr);
	}
	return connfd;
}

void Socket::SetReuseAddr(bool on)
{
	int optval = on ? 1 : 0;
	setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}