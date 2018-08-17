#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <functional>
#include "inetaddress.h"
#include "buffer.h"

class EventItem;
class EventLoop;
class Socket;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
	typedef std::function<void(const TcpConnectionPtr&, Buffer* buf)> MessageCallback;
	typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

	TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& local_addr, const InetAddress& peer_addr);
	~TcpConnection();

	EventLoop* GetLoop() const { return loop_; }
	const std::string& GetName() const { return name_; }
	const InetAddress& GetLocalAddr() { return local_addr_; }
	const InetAddress& GetPeerAddr() { return peer_addr_; }
	bool IsConnected() const { return state_ == Connected; }

	void Send(const std::string& message);
	void Shutdown();

	void SetConnectionCallback(const ConnectionCallback& cb) { connection_callback_ = cb; }
	void SetMessageCallback(const MessageCallback& cb) { message_callback_ = cb; }
	void SetCloseCallback(const CloseCallback& cb) { close_callback_ = cb; }

	void ConnectEstablished();
	void ConnectDestroyed();

private:
	enum Status { Connecting, Connected, Disconnecting, Disconnected};
	void SetState(Status s) { state_ = s; }

	void HandleRead();
	void HandleWrite();
	void HandleClose();
	void HandleError();

	void SendInLoop(const std::string& message);
	void ShutdownInLoop();

	EventLoop* loop_;
	std::string name_;
	Status state_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<EventItem> eventitem_;
	InetAddress local_addr_;
	InetAddress peer_addr_;
	ConnectionCallback connection_callback_;
	MessageCallback message_callback_;
	CloseCallback close_callback_;

	Buffer input_buffer_;
	Buffer output_buffer_;
};

#endif // TCP_CONNECTION_H