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

void TcpConnection::Send(const std::string& message)
{
	if (state_ == Connected)
	{
		if (loop_->IsInBirthThread())
		{
			SendInLoop(message);
		}
		else
		{
			loop_->RunInLoop(std::bind(&TcpConnection::SendInLoop, this, message));
		}
	}
}

void TcpConnection::SendInLoop(const std::string& message)
{
	loop_->AssertInBirthThread();
	ssize_t n_wrote = 0;
	// writing directly when output buffer is empty
	if (!eventitem_->IsWriting() && output_buffer_.GetReadableSize() == 0)
	{
		n_wrote = write(eventitem_->GetFd(), message.data(), message.size());
		if (n_wrote >= 0)
		{
			if ((size_t)n_wrote < message.size())
			{
				LOG_TRACE << "need to write again";
			}
			else if (write_complete_callback_)
			{
				loop_->QueueInLoop(std::bind(write_complete_callback_, shared_from_this()));
			}
		}
		else
		{
			n_wrote = 0;
			if (errno != EWOULDBLOCK)
			{
				LOG_SYSERR << "write error";
			}
		}
	}

	if ((size_t)n_wrote < message.size())
	{
		output_buffer_.Append(message.data() + n_wrote, message.size() - n_wrote);
		if (!eventitem_->IsWriting())
		{
			eventitem_->EnableWriting();
		}
	}
}

void TcpConnection::Shutdown()
{
	if (state_ == Connected)
	{
		SetState(Disconnecting);
		loop_->RunInLoop(std::bind(&TcpConnection::ShutdownInLoop, this));
	}
}

void TcpConnection::ShutdownInLoop()
{
	loop_->AssertInBirthThread();
	if (!eventitem_->IsWriting())
	{
		socket_->ShutdownWrite();
	}
}

void TcpConnection::SetTcpNoDelay(bool on)
{
	socket_->SetTcpNoDelay(on);
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
	int saved_errno = 0;
	ssize_t n = input_buffer_.ReadFd(eventitem_->GetFd(), &saved_errno);
	if (n > 0)
	{
		message_callback_(shared_from_this(), &input_buffer_);
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
	loop_->AssertInBirthThread();
	if (eventitem_->IsWriting())
	{
		ssize_t n = write(eventitem_->GetFd(), output_buffer_.GetReadablePtr(), output_buffer_.GetReadableSize());
		if (n > 0)
		{
			output_buffer_.Retrieve(n);
			if (output_buffer_.GetReadableSize() == 0)
			{
				eventitem_->DisableWriting();
				if (write_complete_callback_)
				{
					loop_->QueueInLoop(std::bind(write_complete_callback_, shared_from_this()));
				}
				if (state_ == Disconnecting)
				{
					ShutdownInLoop();
				}
			}
			else
			{
				LOG_TRACE << "need to write again";
			}
		}
		else
		{
			LOG_SYSERR << "write error";
			abort();
		}
	}
	else
	{
		LOG_TRACE << "Connection is down, no more writing";
	}
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