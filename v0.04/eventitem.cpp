#include "eventitem.h"
#include <sys/epoll.h>
#include "eventloop.h"

EventItem::EventItem(EventLoop* loop, int fd)
	: loop_(loop),
	  fd_(fd),
	  concern_event_types_(0),
	  active_event_types_(0),
	  status_(-1) {}

EventItem::~EventItem() {}

void EventItem::HandleEvent()
{
	if (active_event_types_ & (EPOLLIN | EPOLLPRI)) {
		if (read_callback_) read_callback_();
	}
}

void EventItem::EnableReading() 
{ 
    concern_event_types_ |= (EPOLLIN | EPOLLPRI);
    Update();
}

void EventItem:: Update()
{
	loop_->UpdateEventItem(this);
}