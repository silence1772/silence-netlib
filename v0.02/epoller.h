#ifndef EPOLLER_H
#define EPOLLER_H

#include <vector>
#include <map>

// 前向声明
struct epoll_event;
class Channel;
class EventLoop;

class Epoller
{
public:
	typedef std::vector<Channel*> ChannelList;

	Epoller();
	~Epoller();
	
	int Poll(ChannelList* active_channels);
	void UpdateChannel(Channel* channel);

private:
	void Update(int operation, Channel* channel);
	void FillActiveChannels(int num_events, ChannelList* active_channels) const;
	
	typedef std::map<int, Channel*> ChannelMap;
	typedef std::vector<struct epoll_event> EventList;

	ChannelMap channels_;
	EventList events_;

	int epollfd_;
};

#endif // EPOLL_H