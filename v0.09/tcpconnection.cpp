#include "tcpconnection.h"
#include <unistd.h>
#include <errno.h>
#include "eventloop.h"
#include "eventitem.h"
#include "socket.h"
#include "socketops.h"
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
	eventitem_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
	eventitem_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
	eventitem_->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
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

void TcpConnection::ConnectDestroyed()
{
	loop_->AssertInBirthThread();
	SetState(Disconnected);
	eventitem_->DisableAll();
	connection_callback_(shared_from_this());
	loop_->RemoveEventItem(eventitem_.get());
}

void TcpConnection::HandleRead()
{
	char buf[65536];
	ssize_t n = read(eventitem_->GetFd(), buf, sizeof(buf));
	if (n > 0)
	{
		message_callback_(shared_from_this(), buf, n);
	}
	else if (n == 0)
	{
		HandleClose();
	}
	else
	{
		HandleError();
	}
}

void TcpConnection::HandleWrite()
{

}

void TcpConnection::HandleClose()
{
	loop_->AssertInBirthThread();
	eventitem_->DisableAll();
	close_callback_(shared_from_this());
}

void TcpConnection::HandleError()
{
	int err = socketops::GetSocketError(eventitem_->GetFd());
	LOG_ERROR << "HandleError [" << name_ << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}