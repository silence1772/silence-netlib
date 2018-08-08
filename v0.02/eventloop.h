#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <vector>
#include <memory>

class Channel;
class Epoller;

class EventLoop
{
public:
	EventLoop();
	~EventLoop();
	
	void Loop();
	void Quit() { quit_ = true; }
	void UpdateChannel(Channel* channel);

private:
	typedef std::vector<Channel*> ChannelList;

	bool quit_;
	ChannelList active_channels_;
	std::unique_ptr<Epoller> epoller_;
};

#endif // EVENTLOOP_H
