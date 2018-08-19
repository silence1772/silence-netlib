#include "tcpclient.h"
#include "connector.h"
#include "eventloop.h"
#include "socketops.h"
#include "log/logger.h"

TcpClient::TcpClient(EventLoop* loop, const InetAddress& server_addr)
	: loop_(loop),
	  connector_(new Connector(loop, server_addr)),
	  retry_(false),
	  connect_(true),
	  next_conn_id_(1)
{
	connector_->SetNewConnectionCallback(std::bind(&TcpClient::HandleNewConnection, this, std::placeholders::_1));
}

TcpClient::~TcpClient()
{
	TcpConnectionPtr conn;

	{
		MutexLockGuard lock(mutex_);
		conn = connection_;
	}
	if (conn)
	{
		loop_->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
	}
	else
	{
		connector_->Stop();
	}
}

void TcpClient::Connect()
{
	LOG_INFO << this << " connecting to " << connector_->GetServerAddr().ToHostPort();
	connect_ = true;
	connector_->Start();
}

void TcpClient::Disconnect()
{
	connect_ = false;
	{
		MutexLockGuard lock(mutex_);
		if (connection_)
		{
			connection_->Shutdown();
		}
	}
}

void TcpClient::Stop()
{
	connect_ = false;
	connector_->Stop();
}

void TcpClient::HandleNewConnection(int sockfd)
{
	loop_->AssertInBirthThread();
	InetAddress peer_addr(socketops::GetPeerAddr(sockfd));

	char buf[32];
	snprintf(buf, sizeof(buf), ":%s#%d", peer_addr.ToHostPort().c_str(), next_conn_id_++);
	string conn_name = buf;

	InetAddress local_addr(socketops::GetLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnection(loop_, conn_name, sockfd, local_addr, peer_addr));
	conn->SetConnectionCallback(connection_callback_);
	conn->SetMessageCallback(message_callback_);
	conn->SetWriteCompleteCallback(write_complete_callback_);
	conn->SetCloseCallback(std::bind(&TcpClient::RemoveConnection, this, std::placeholders::_1));

	{
		MutexLockGuard lock(mutex_);
		connection_ = conn;
	}
	conn->ConnectEstablished();
}

void TcpClient::RemoveConnection(const TcpConnectionPtr& conn)
{
	loop_->AssertInBirthThread();
	{
		MutexLockGuard lock(mutex_);
		connection_.reset();
	}
	loop_->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
	if (retry_ && connect_)
	{
		LOG_INFO << "Reconnecting...";
		connector_->Restart();
	}
}