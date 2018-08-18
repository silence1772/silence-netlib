#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include "eventitem.h"
#include "socket.h"

class EventLoop;
class InetAddress;

class Acceptor
{
public:
	typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop* loop, const InetAddress& addr);

	void SetNewConnectionCallback(const NewConnectionCallback& cb) { new_connection_callback_ = cb; }
	
	bool IsListenning() const { return is_listenning; }

	void Listen();
private:
	void HandelNewConnection();

	EventLoop* loop_;
	Socket accept_socket_;
	EventItem accept_eventitem_;
	NewConnectionCallback new_connection_callback_;
	bool is_listenning;
};

#endif // ACCEPTOR_H