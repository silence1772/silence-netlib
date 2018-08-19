#include "epoller.h"
#include <sys/epoll.h>
#include <string.h> // for bzero()
#include "eventitem.h"
#include "log/logger.h"

Epoller::Epoller()
	: epollfd_(epoll_create1(EPOLL_CLOEXEC)),
	  events_(10) {}
Epoller::~Epoller() {}

int Epoller::Poll(EventItemList* active_eventitems)
{
	int active_event_num = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), 5000);
	if (active_event_num > 0)
	{
		LOG_TRACE << active_event_num << " enents happened";
		FillActiveEventItems(active_event_num, active_eventitems);
		if (size_t(active_event_num) == events_.size())
    	{
      		events_.resize(events_.size() * 2);
		}
	}
	else if (active_event_num == 0)
	{
		LOG_TRACE << "nothin happened";
	}
	else
	{
		LOG_SYSERR << "Epoller::Poll()";
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
	}
	else if (status == 1) // Disable
	{
		eventitem->SetStatus(0);
		Update(EPOLL_CTL_ADD, eventitem);
	}
	else // Added
	{
		if (eventitem->IsNoneEvent())
		{
			Update(EPOLL_CTL_DEL, eventitem);
			eventitem->SetStatus(1);
		}
		else
		{
			Update(EPOLL_CTL_MOD, eventitem);
		}
	}
}

void Epoller::RemoveEventItem(EventItem* eventitem)
{
	int fd = eventitem->GetFd();
	int status = eventitem->GetStatus();

	eventitems_.erase(fd);
	if (status == 0) //added
	{
		Update(EPOLL_CTL_DEL, eventitem);
	}
	eventitem->SetStatus(-1);
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
  		LOG_SYSERR << "Epoller::Update() epoll_ctl";
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