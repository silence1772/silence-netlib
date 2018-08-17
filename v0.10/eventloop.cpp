#include "eventloop.h"
#include <sys/time.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include "eventitem.h"
#include "epoller.h"
#include "timerqueue.h"
#include "timestamp.h"
#include "currentthread.h"
#include "log/logger.h"

typedef std::function<void()> TimerCallback;
typedef std::function<void()> Functor;

static int CreateEventfd()
{
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		LOG_SYSERR << "CreateEventfd()";
	}
  return evtfd;
}

EventLoop::EventLoop()
	: quit_(false),
	  thread_id_(GetTid()),
	  epoller_(new Epoller()),
	  timer_queue_(new TimerQueue(this)),
	  wakeup_fd_(CreateEventfd()),
	  is_handling_functor_queue_(false),
	  wakeup_eventitem_(new EventItem(this, wakeup_fd_))

{
	wakeup_eventitem_->SetReadCallback(std::bind(&EventLoop::HandleWakeup, this));
	wakeup_eventitem_->EnableReading();
}

EventLoop::~EventLoop()
{

}

void EventLoop::Loop()
{
	AssertInBirthThread();
	while (!quit_)
	{
		active_eventitems_.clear();
		epoller_->Poll(&active_eventitems_);
		for (auto it = active_eventitems_.begin(); it != active_eventitems_.end(); ++it)
		{
			(*it)->HandleEvent();
		}
		DoFunctorQueue();
	}
	LOG_TRACE << "EventLoop " << this << " stop looping";

}

void EventLoop::UpdateEventItem(EventItem* eventitem)
{
	epoller_->UpdateEventItem(eventitem);
}

void EventLoop::RemoveEventItem(EventItem* eventitem)
{
	epoller_->RemoveEventItem(eventitem);
}

void EventLoop::AssertInBirthThread()
{
	if (!IsInBirthThread())
	{
		LOG_FATAL << "EventLoop::AssertInBirthThread() - EventLoop " << this
				  << " was created in thread id = " << thread_id_
				  << ", current thread id = " << GetTid();
	}
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

void EventLoop::RunInLoop(Functor func)
{
	if (IsInBirthThread())
	{
		func();
	}
	else
	{
		QueueInLoop(std::move(func));
	}
}

void EventLoop::QueueInLoop(Functor func)
{
	{
		MutexLockGuard lock(mutex_);
		functor_queue_.push_back(std::move(func));
	}
	
	if (!IsInBirthThread() || is_handling_functor_queue_)
	{
		Wakeup();
	}
}

void EventLoop::Wakeup()
{
	uint64_t one = 1;
	ssize_t n = write(wakeup_fd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG_SYSERR << "write error";
	}
}

void EventLoop::HandleWakeup()
{
	uint64_t one = 1;
	ssize_t n = read(wakeup_fd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG_SYSERR << "read error";
	}
}

void EventLoop::DoFunctorQueue()
{
	is_handling_functor_queue_ = true;
	std::vector<Functor> functors;
	{
		MutexLockGuard lock(mutex_);
		functors.swap(functor_queue_);
	}

	for(auto i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}
	is_handling_functor_queue_ = false;
}