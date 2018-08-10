#include "eventloop.h"
#include <sys/time.h>
#include <iostream>
#include "eventitem.h"
#include "epoller.h"
#include "timerqueue.h"
#include "timestamp.h"

typedef std::function<void()> TimerCallback;

EventLoop::EventLoop()
	: quit_(false),
	  epoller_(new Epoller()),
	  timer_queue_(new TimerQueue(this))
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

void EventLoop::RunAt(const Timestamp& timestamp, const TimerCallback& cb)
{
	timer_queue_->AddTimer(cb, timestamp);
}

void EventLoop::RunAfter(double delay, const TimerCallback& cb)
{
  	timer_queue_->AddTimer(cb, Timestamp::GetAfter(delay));
}

void EventLoop::RunEvery(double interval, const TimerCallback& cb)
{
	timer_queue_->AddTimer(cb, Timestamp::GetAfter(interval), interval);
}