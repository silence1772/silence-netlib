#include "connector.h"
#include <errno.h>
#include "eventitem.h"
#include "eventloop.h"
#include "socketops.h"
#include "inetaddress.h"
#include "log/logger.h"

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& server_addr)
	: loop_(loop),
	  server_addr_(server_addr),
	  is_connect_(false),
	  state_(kDisconnected),
	  retry_delay_ms_(kInitRetryDelayMs) { }

Connector::~Connector()
{

}

void Connector::Start()
{
	is_connect_ = true;
	loop_->RunInLoop(std::bind(&Connector::StartInLoop, this));
}

void Connector::Restart()
{
	loop_->AssertInBirthThread();
	SetState(kDisconnected);
	retry_delay_ms_ = kInitRetryDelayMs;
	is_connect_ = true;
	StartInLoop();
}

void Connector::StartInLoop()
{
	loop_->AssertInBirthThread();
	if (is_connect_)
	{
		Connect();
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}
}

void Connector::Stop()
{
	is_connect_ = false;
	loop_->QueueInLoop(std::bind(&Connector::StopInLoop, this));
}

void Connector::StopInLoop()
{
	loop_->AssertInBirthThread();
	if (state_ == kConnecting)
	{
		SetState(kDisconnected);
		int sockfd = RemoveAndResetEventitem();
		Retry(sockfd);
	}
}

void Connector::Connect()
{
	int sockfd = socketops::CreateNonblocking();
	int ret = socketops::Connect(sockfd, server_addr_.GetSockAddrInet());
	int saved_errno = (ret == 0) ? 0 : errno;
	switch (saved_errno)
	{
	case 0:
	case EINPROGRESS:
	case EINTR:
	case EISCONN:
		Connecting(sockfd);
		break;

	case EAGAIN:
	case EADDRINUSE:
	case EADDRNOTAVAIL:
	case ECONNREFUSED:
	case ENETUNREACH:
		Retry(sockfd);
		break;

	case EACCES:
	case EPERM:
	case EAFNOSUPPORT:
	case EALREADY:
	case EBADF:
	case EFAULT:
	case ENOTSOCK:
		LOG_SYSERR << "connect error " << saved_errno;
		socketops::Close(sockfd);
		break;

	default:
		LOG_SYSERR << "Unexpected error " << saved_errno;
		socketops::Close(sockfd);
		break;
	}
}

void Connector::Connecting(int sockfd)
{
	SetState(kConnecting);
	eventitem_.reset(new EventItem(loop_, sockfd));
	eventitem_->SetWriteCallback(std::bind(&Connector::HandleWrite, this));
	eventitem_->SetErrorCallback(std::bind(&Connector::HandleError, this));

	eventitem_->EnableWriting();
}

void Connector::HandleWrite()
{
	if (state_ == kConnecting)
	{
		int sockfd = RemoveAndResetEventitem();
		int err = socketops::GetSocketError(sockfd);
		if (err)
		{
			LOG_WARN << "Connector::handleWrite - SO_ERROR = " << err << " " << strerror_tl(err);
			Retry(sockfd);
		}
		else if (socketops::IsSelfConnect(sockfd))
		{
			LOG_WARN << "Connector::handleWrite - Self connect";
			Retry(sockfd);
		}
		else
		{
			SetState(kConnected);
			if (is_connect_)
			{
				new_connection_callback_(sockfd);
			}
			else
			{
				socketops::Close(sockfd);
			}
		}
	}
}

void Connector::HandleError()
{
	LOG_ERROR << "Connector::handleError state=" << state_;
	if (state_ == kConnecting)
	{
		int sockfd = RemoveAndResetEventitem();
		int err = socketops::GetSocketError(sockfd);
		LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
		Retry(sockfd);
	}
}

void Connector::Retry(int sockfd)
{
	socketops::Close(sockfd);
	SetState(kDisconnected);
	if (is_connect_)
	{
		LOG_INFO << "Connector::retry - Retry connecting to " << server_addr_.ToHostPort() << " in " << retry_delay_ms_ << " milliseconds. ";
		loop_->RunAfter(retry_delay_ms_ * 1000.0, std::bind(&Connector::StartInLoop, shared_from_this()));
		retry_delay_ms_ = std::min(retry_delay_ms_ * 2, kMaxRetryDelayMs);
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}
}

int Connector::RemoveAndResetEventitem()
{
	eventitem_->DisableAll();
	eventitem_->Remove();
	int sockfd = eventitem_->GetFd();
	// Can't reset eventitem_ here, because we are inside Channel::handleEvent
	loop_->QueueInLoop(std::bind(&Connector::ResetEventitem, this)); 
	return sockfd;
}

void Connector::ResetEventitem()
{
	eventitem_.reset();
}