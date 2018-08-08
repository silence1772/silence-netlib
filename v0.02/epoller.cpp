#include "epoller.h"
#include <sys/epoll.h>
#include <string.h> // for bzero()
#include <iostream>
#include "channel.h"

Epoller::Epoller()
	: epollfd_(epoll_create1(EPOLL_CLOEXEC)),
	  events_(10) {}
Epoller::~Epoller() {}

int Epoller::Poll(ChannelList* active_channels)
{
	int active_event_num = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), 5000);
	if (active_event_num > 0)
	{
		std::cout << "There are " << active_event_num << " event(s) happened" << std::endl;
		FillActiveChannels(active_event_num, active_channels);
		if (size_t(active_event_num) == events_.size())
    	{
      		events_.resize(events_.size() * 2);
		}
	}
	else if (active_event_num == 0)
	{
		std::cout << "Polling timeout and Nothing happend" << std::endl;
	}
	else
	{
		std::cout << "epoll_wait error" << std::endl;
	}
	return active_event_num;
}

void Epoller::UpdateChannel(Channel* channel)
{
	const int status = channel->GetStatus();
	const int fd = channel->GetFd();
	if (status == -1) // New
	{
		channels_[fd] = channel;
		channel->SetStatus(0);
		Update(EPOLL_CTL_ADD, channel);
		std::cout << "The channel with fd " << fd << " is registered to Epoller" << std::endl;
	}
	else // Added
	{
		Update(EPOLL_CTL_MOD, channel);
		std::cout << "The channel with fd " << fd << " is modified" << std::endl;
	}
}

void Epoller::Update(int operation, Channel* channel)
{
	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = channel->GetEvents();
  	event.data.ptr = channel;
  	int fd = channel->GetFd();
  	if (epoll_ctl(epollfd_, operation, fd, &event) < 0)
  	{
  		std::cout << "epoll_ctl error" << std::endl;
  	}
}

void Epoller::FillActiveChannels(int active_event_num, ChannelList* active_channels) const
{
	for (int i = 0; i < active_event_num; ++i)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->SetRevents(events_[i].events);
		active_channels->push_back(channel);
	}
}