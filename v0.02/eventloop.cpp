#include "eventloop.h"
#include <iostream>
#include "channel.h"
#include "epoller.h"

EventLoop::EventLoop()
	: quit_(false),
	  epoller_(new Epoller())
{
	std::cout << "EventLoop " << this << " created" << std::endl;
}

EventLoop::~EventLoop()
{
	std::cout << "EventLoop: Bye~~" << std::endl;
}

void EventLoop::Loop()
{
	std::cout << "EventLoop " << this << " start looping" << std::endl;
	while (!quit_)
	{
		active_channels_.clear();
		std::cout << "Waitting poll......" << std::endl;
		epoller_->Poll(&active_channels_);
		for (auto it = active_channels_.begin(); it != active_channels_.end(); ++it)
		{
			(*it)->HandleEvent();
		}
	}
	std::cout << "EventLoop " << this << " stop looping" << std::endl;

}

void EventLoop::UpdateChannel(Channel* channel)
{
	epoller_->UpdateChannel(channel);
}
