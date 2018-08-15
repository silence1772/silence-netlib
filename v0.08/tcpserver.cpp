#include "tcpserver.h"
#include "acceptor.h"
#include "socketops.h"
#include "eventloop.h"
#include "log/logger.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddress& addr)
	: loop_(loop),
	  name_(addr.ToHostPort()),
	  acceptor_(new Acceptor(loop, addr)),
	  is_started_(false),
	  next_connid_(0)
{
	acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::HandleNewConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::Start()
{
	if (!is_started_)
		is_started_ = true;
	if (!acceptor_->IsListenning())
	{
		loop_->RunInLoop(std::bind(&Acceptor::Listen, acceptor_.get()));
	}
}

void TcpServer::HandleNewConnection(int sockfd, const InetAddress& peer_addr)
{
	loop_->AssertInBirthThread();
	char buf[32];
	snprintf(buf, sizeof(buf), "#%d", next_connid_++);
	std::string conn_name = name_ + buf;
	LOG_INFO << "new connection [" << conn_name << "] from " << peer_addr.ToHostPort();

	InetAddress local_addr(socketops::GetLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnection(loop_, conn_name, sockfd, local_addr, peer_addr));
	connections_[conn_name] = conn;
	conn->SetConnectionCallback(connection_callback_);
	conn->SetMessageCallback(message_callback_);
	conn->ConnectEstablished();
}