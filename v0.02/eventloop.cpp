#include "eventloop.h"
#include <iostream>
#include "eventitem.h"
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
		active_eventitems_.clear();
		std::cout << "Waitting poll......" << std::endl;
		epoller_->Poll(&active_eventitems_);
		for (auto it = active_eventitems_.begin(); it != active_eventitems_.end(); ++it)
		{
			(*it)->HandleEvent();
		}
	}
	std::cout << "EventLoop " << this << " stop looping" << std::endl;

}

void EventLoop::UpdateEventItem(EventItem* eventitem)
{
	epoller_->UpdateEventItem(eventitem);
}
