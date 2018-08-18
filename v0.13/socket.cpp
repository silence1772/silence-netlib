#include "socket.h"
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "inetaddress.h"
#include "socketops.h"

Socket::~Socket()
{
	socketops::Close(sockfd_);
}

void Socket::Bind(const InetAddress& addr)
{
	socketops::Bind(sockfd_, addr.GetSockAddrInet());
}

void Socket::Listen()
{
	socketops::Listen(sockfd_);
}

int Socket::Accept(InetAddress* peer_addr)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	int connfd = socketops::Accept(sockfd_, &addr);
	if (connfd >= 0)
	{
		peer_addr->SetSockAddrInet(addr);
	}
	return connfd;
}

void Socket::ShutdownWrite()
{
	socketops::Shutdown(sockfd_);
}

void Socket::SetReuseAddr(bool on)
{
	int optval = on ? 1 : 0;
	setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::SetTcpNoDelay(bool on)
{
	int optval = on ? 1 : 0;
	setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::SetKeepAlive(bool on)
{
	int optval = on ? 1 : 0;
	setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}