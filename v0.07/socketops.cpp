#include "socketops.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  // snprintf
#include <strings.h>  // bzero
#include <sys/socket.h>
#include <unistd.h>
#include "log/logger.h"

int CreateNonblocking()
{
	int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (sockfd < 0)
	{
		LOG_SYSFATAL << "Create error";
	}
	return sockfd;
}

void Bind(int sockfd, const struct sockaddr_in& addr)
{
	int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		LOG_SYSFATAL << "Bind error";
	}
}

void Listen(int sockfd)
{
	int ret = listen(sockfd, SOMAXCONN);
	if (ret < 0)
	{
		LOG_SYSFATAL << "Listen error";
	}
}

int Accept(int sockfd, struct sockaddr_in* addr)
{
	socklen_t addrlen = sizeof(*addr);
	int connfd = accept4(sockfd, (struct sockaddr*)addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (connfd < 0)
	{
		int saved_errno = errno;
		LOG_SYSERR << "Accept error";
		switch (savedErrno)
		{
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO: 
			case EPERM:
			case EMFILE: 
				errno = saved_errno;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				LOG_FATAL << "unexpected error of Accept " << saved_errno;
				break;
			default:
				LOG_FATAL << "unknown error of Accept " << saved_errno;
				break;
		}
	}
	return connfd;
}

void Close(int sockfd)
{
	if (close(sockfd) < 0)
	{
		LOG_SYSERR << "Close error";
	}
}

void ToHostPort(char* buf, size_t size, const struct sockaddr_in& addr)
{
	char host[INET_ADDRSTRLEN] = "INVALID";
	inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
	uint16_t port = NetworkToHost16(addr.sin_port);
	snprintf(buf, size, "%s:%u", host, port);
}

void FromHostPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = HostToNetwork16(port);
	if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
		LOG_SYSERR << "FromHostPort() error";
	}
}