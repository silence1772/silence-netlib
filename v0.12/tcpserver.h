#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "tcpconnection.h"
#include <map>

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer* buf)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;


class Acceptor;
class EventLoop;

class TcpServer
{
public:
	TcpServer(EventLoop* loop, const InetAddress& addr);
	~TcpServer();

	void Start();

	void SetConnectionCallback(const ConnectionCallback& cb) { connection_callback_ = cb; }
	void SetMessageCallback(const MessageCallback& cb) { message_callback_ = cb; }
	void SetWriteCompleteCallback(const WriteCompleteCallback& cb) { write_complete_callback_ = cb; }
private:
	void HandleNewConnection(int sockfd, const InetAddress& peer_addr);
	void RemoveConnection(const TcpConnectionPtr& conn);

	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

	EventLoop* loop_;
	const std::string name_;
	std::unique_ptr<Acceptor> acceptor_;
	bool is_started_;
	ConnectionCallback connection_callback_;
	MessageCallback message_callback_;
	WriteCompleteCallback write_complete_callback_;
	ConnectionMap connections_;
	int next_connid_;
};

#endif // TCP_SERVER_H