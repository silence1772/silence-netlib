#include "tcpserver.h"
#include "acceptor.h"
#include "socketops.h"
#include "eventloop.h"
#include "eventloopthreadpool.h"
#include "log/logger.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddress& addr)
	: loop_(loop),
	  name_(addr.ToHostPort()),
	  acceptor_(new Acceptor(loop, addr)),
	  thread_pool_(new EventLoopThreadPool(loop)),
	  is_started_(false),
	  next_connid_(0)
{
	acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::HandleNewConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::SetThreadNum(int thread_num)
{
	thread_pool_->SetThreadNum(thread_num);
}

void TcpServer::Start()
{
	if (!is_started_)
	{
		is_started_ = true;
		thread_pool_->Start();
	}
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

	EventLoop* io_loop = thread_pool_->GetNextLoop();

	TcpConnectionPtr conn(new TcpConnection(io_loop, conn_name, sockfd, local_addr, peer_addr));
	connections_[conn_name] = conn;
	conn->SetConnectionCallback(connection_callback_);
	conn->SetMessageCallback(message_callback_);
	conn->SetWriteCompleteCallback(write_complete_callback_);
	conn->SetCloseCallback(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
	
	io_loop->RunInLoop(std::bind(&TcpConnection::ConnectEstablished, conn));
}

void TcpServer::RemoveConnection(const TcpConnectionPtr& conn)
{
	loop_->RunInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));	
}

void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_->AssertInBirthThread();
	LOG_INFO << "remove connection [" << conn->GetName() << "] from " << conn->GetPeerAddr().ToHostPort();
	size_t n = connections_.erase(conn->GetName());
	EventLoop* io_loop = conn->GetLoop();
	io_loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
}