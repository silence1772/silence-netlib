#include "acceptor.h"
// #include <errno.h>
// #include <fcntl.h>
// #include <unistd.h>
#include "eventloop.h"
#include "inetaddress.h"
#include "socketops.h"
#include "log/logger.h"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr)
	: loop_(loop),
	  accept_socket_(socketops::CreateNonblocking()),
	  accept_eventitem_(loop, accept_socket_.GetSockfd()),
	  is_listenning(false)
{
	accept_socket_.SetReuseAddr(true);
	accept_socket_.Bind(addr);
	accept_eventitem_.SetReadCallback(std::bind(&Acceptor::HandelNewConnection, this));
}

void Acceptor::Listen()
{
	loop_->AssertInBirthThread();
	is_listenning = true;
	accept_socket_.Listen();
	accept_eventitem_.EnableReading();
}

void Acceptor::HandelNewConnection()
{
	loop_->AssertInBirthThread();
	InetAddress peer_addr(0);
	int connfd = accept_socket_.Accept(&peer_addr);
	if (connfd >= 0)
	{
		if (new_connection_callback_)
		{
			new_connection_callback_(connfd, peer_addr);
		}
		else
		{
			socketops::Close(connfd);
		}
	}
	else
	{
		LOG_SYSERR << "Accept error";
	}
}