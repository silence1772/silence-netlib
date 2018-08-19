#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "mutex.h"
#include "tcpconnection.h"

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer* buf)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;

class TcpClient
{
public:
	TcpClient(EventLoop* loop, const InetAddress& server_addr);
	~TcpClient();

	void Connect();
	void Disconnect();
	void Stop();

	TcpConnectionPtr GetConnection() const
	{
		MutexLockGuard lock(mutex_);
		return connection_;
	}

	bool GetRetry() const { return retry_; }
	void EnableRetry() { retry_ = true; }

	void SetConnectionCallback(const ConnectionCallback& cb) { connection_callback_ = cb; }
	void SetMessageCallback(const MessageCallback& cb) { message_callback_ = cb; }
	void SetWriteCompleteCallback(const WriteCompleteCallback& cb) { write_complete_callback_ = cb; }

private:
	void HandleNewConnection(int sockfd);
	void RemoveConnection(const TcpConnectionPtr& conn);

	EventLoop* loop_;
	ConnectorPtr connector_;
	ConnectionCallback connection_callback_;
	MessageCallback message_callback_;
	WriteCompleteCallback write_complete_callback_;
	bool retry_;
	bool connect_;
	int next_conn_id_;
	mutable MutexLock mutex_;
	TcpConnectionPtr connection_;
};

#endif // TCPCLIENT_H