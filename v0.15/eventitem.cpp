#include "eventitem.h"
#include <sys/epoll.h>
#include "eventloop.h"
#include "log/logger.h"

EventItem::EventItem(EventLoop* loop, int fd)
	: loop_(loop),
	  fd_(fd),
	  concern_event_types_(0),
	  active_event_types_(0),
	  status_(-1) {}

EventItem::~EventItem() {}

void EventItem::HandleEvent()
{
	if ((active_event_types_ & EPOLLHUP) && !(active_event_types_ & EPOLLIN))
	{
		LOG_WARN << "POLLHUP";
		if (close_callback_) close_callback_();
	}

	if (active_event_types_ & EPOLLERR)
	{
		if (error_callback_) error_callback_();
	}

	if (active_event_types_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		if (read_callback_) read_callback_();
	}

	if (active_event_types_ & EPOLLOUT)
	{
		if (write_callback_) write_callback_();
	}
}

void EventItem::EnableReading() 
{ 
    concern_event_types_ |= (EPOLLIN | EPOLLPRI);
    Update();
}

void EventItem::EnableWriting()
{
	concern_event_types_ |= EPOLLOUT;
	Update();
}

void EventItem::DisableWriting()
{
	concern_event_types_ &= ~EPOLLOUT;
	Update();
}

void EventItem::DisableAll()
{
	concern_event_types_ = 0;
	Update();
}

void EventItem::Remove()
{
	loop_->RemoveEventItem(this);
}

void EventItem::Update()
{
	loop_->UpdateEventItem(this);
}