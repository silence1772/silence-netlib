#include "epoller.h"
#include <sys/epoll.h>
#include <string.h> // for bzero()
#include <iostream>
#include "eventitem.h"

Epoller::Epoller()
	: epollfd_(epoll_create1(EPOLL_CLOEXEC)),
	  events_(10) {}
Epoller::~Epoller() {}

int Epoller::Poll(EventItemList* active_eventitems)
{
	int active_event_num = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), 5000);
	if (active_event_num > 0)
	{
		std::cout << "There are " << active_event_num << " event(s) happened" << std::endl;
		FillActiveEventItems(active_event_num, active_eventitems);
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

void Epoller::UpdateEventItem(EventItem* eventitem)
{
	const int status = eventitem->GetStatus();
	const int fd = eventitem->GetFd();
	if (status == -1) // New
	{
		eventitems_[fd] = eventitem;
		eventitem->SetStatus(0);
		Update(EPOLL_CTL_ADD, eventitem);
		std::cout << "The EventItem with fd " << fd << " is registered to Epoller" << std::endl;
	}
	else // Added
	{
		Update(EPOLL_CTL_MOD, eventitem);
		std::cout << "The EventItem with fd " << fd << " is modified" << std::endl;
	}
}

void Epoller::Update(int operation, EventItem* eventitem)
{
	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = eventitem->GetConcernEventTypes();
  	event.data.ptr = eventitem;
  	int fd = eventitem->GetFd();
  	if (epoll_ctl(epollfd_, operation, fd, &event) < 0)
  	{
  		std::cout << "epoll_ctl error" << std::endl;
  	}
}

void Epoller::FillActiveEventItems(int active_event_num, EventItemList* active_eventitems) const
{
	for (int i = 0; i < active_event_num; ++i)
	{
		EventItem* eventitem = static_cast<EventItem*>(events_[i].data.ptr);
		eventitem->SetActiveEventTypes(events_[i].events);
		active_eventitems->push_back(eventitem);
	}
}