#include "tcpconnection.h"
#include <unistd.h>
#include "eventloop.h"
#include "eventitem.h"
#include "socket.h"
#include "log/logger.h"

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& local_addr, const InetAddress& peer_addr)
	: loop_(loop),
	  name_(name),
	  state_(Connecting),
	  socket_(new Socket(sockfd)),
	  eventitem_(new EventItem(loop, sockfd)),
	  local_addr_(local_addr),
	  peer_addr_(peer_addr)
{
	eventitem_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::ConnectEstablished()
{
	loop_->AssertInBirthThread();
	SetState(Connected);
	eventitem_->EnableReading();
	connection_callback_(shared_from_this());
}

void TcpConnection::HandleRead()
{
	char buf[65536];
	ssize_t n = read(eventitem_->GetFd(), buf, sizeof(buf));
	message_callback_(shared_from_this(), buf, n);
}