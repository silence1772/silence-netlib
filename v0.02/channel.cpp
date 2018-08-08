#include "channel.h"
#include <sys/epoll.h>
#include "eventloop.h"

Channel::Channel(EventLoop* loop, int fd)
	: loop_(loop),
	  fd_(fd),
	  events_(0),
	  revents_(0),
	  status_(-1) {}

Channel::~Channel() {}

void Channel::HandleEvent()
{
	if (revents_ & (EPOLLIN | EPOLLPRI)) {
		if (read_callback_) read_callback_();
	}
}

void Channel::EnableReading() 
{ 
    events_ |= (EPOLLIN | EPOLLPRI);
    Update();
}

void Channel:: Update()
{
	loop_->UpdateChannel(this);
}