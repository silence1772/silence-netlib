#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <functional>
#include <memory>
#include "inetaddress.h"

class EventItem;
class EventLoop;

class Connector : public std::enable_shared_from_this<Connector>
{
public:
	typedef std::function<void(int sockfd)> NewConnectionCallback;

	Connector(EventLoop* loop, const InetAddress& server_addr);
	~Connector();
	
	void SetNewConnectionCallback(const NewConnectionCallback& cb) { new_connection_callback_ = cb; }

	void Start();
	void Restart();
	void Stop();

	const InetAddress& GetServerAddr() const { return server_addr_; }

private:
	enum Status { kConnecting, kConnected, kDisconnected };
	static const int kMaxRetryDelayMs = 30 * 1000;
	static const int kInitRetryDelayMs = 0.5 * 1000;

	void SetState(Status s) { state_ = s; }
	void StartInLoop();
	void StopInLoop();
	void Connect();
	void Connecting(int sockfd);
	void HandleWrite();
	void HandleError();
	void Retry(int sockfd);
	int RemoveAndResetEventitem();
	void ResetEventitem();

	EventLoop* loop_;
	InetAddress server_addr_;
	bool is_connect_;
	Status state_;
	std::unique_ptr<EventItem> eventitem_;
	NewConnectionCallback new_connection_callback_;
	int retry_delay_ms_;
};

#endif //  CONNECTOR_H