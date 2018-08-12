#include "timerqueue.h"
#include <sys/timerfd.h>
#include <unistd.h> // for close()
#include <string.h> // for bzero()
#include <sys/time.h> // gor gettimeofday()
#include <iostream>
#include "timestamp.h"
#include "timer.h"
#include "eventloop.h"

int CreateTimerfd()
{
	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0)
	{
    	std::cout << "timerfd_create error" << std::endl;
	}
	return timerfd;
}

void ResetTimerfd(int timerfd, Timestamp expiration)
{
	struct itimerspec new_value;
	struct itimerspec old_value;
	bzero(&new_value, sizeof(new_value));
	bzero(&old_value, sizeof(old_value));

  	// get gap between now and future
	int64_t microseconds = expiration.GetMicrosecondTimestamp() - Timestamp::GetNow().GetMicrosecondTimestamp();
	if (microseconds < 100) microseconds = 100;
	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microseconds / (1000 * 1000));
	ts.tv_nsec = static_cast<long>((microseconds % (1000 * 1000)) * 1000);

	// update timerfd's remaining time
	new_value.it_value = ts;
	timerfd_settime(timerfd, 0, &new_value, &old_value);
}


TimerQueue::TimerQueue(EventLoop* loop)
	: loop_(loop),
	  timerfd_(CreateTimerfd()),
	  timerfd_eventitem_(loop, timerfd_),
	  timerlist_()
{
	timerfd_eventitem_.SetReadCallback(std::bind(&TimerQueue::HandelTimerExpired, this));
	timerfd_eventitem_.EnableReading();
}

TimerQueue::~TimerQueue()
{
	close(timerfd_);
	for (auto it = timerlist_.begin(); it != timerlist_.end(); ++it)
	{
		delete it->second;
	}
}

void TimerQueue::AddTimer(const TimerCallback& cb, Timestamp timestamp, double interval)
{
	Timer *timer = new Timer(cb, timestamp, interval);
	loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
}

void TimerQueue::AddTimerInLoop(Timer* timer)
{
	auto it = timerlist_.begin();

	// if the new Timer get timeout before all old Timer
	if (it == timerlist_.end() || timer->GetExpirationTimestamp() < it->first)
	{
		ResetTimerfd(timerfd_, timer->GetExpirationTimestamp());
	}

	timerlist_.insert(std::make_pair(timer->GetExpirationTimestamp(), timer));
}

void TimerQueue::HandelTimerExpired()
{
	// read the timerfd
	uint64_t exp_cnt;
	ssize_t n = read(timerfd_, &exp_cnt, sizeof(exp_cnt));
	if (n != sizeof(exp_cnt)) 
		std::cout << "read error" << std::endl;

	// run the callback
	std::vector<Entry> expired = GetExpired(Timestamp::GetNow());
	for (auto it = expired.begin(); it != expired.end(); ++it)
	{
		it->second->Run();
	}
	// update the repeat Timer
	for (auto it = expired.begin(); it != expired.end(); ++it)
	{
		if (it->second->IsRepeat())
		{
			it->second->Continue();
			timerlist_.insert(std::make_pair(it->second->GetExpirationTimestamp(), it->second));
		}
		else
		{
			delete it->second;
		}
	}
	// update the next timer
	if (!timerlist_.empty())
	{
		ResetTimerfd(timerfd_, timerlist_.begin()->second->GetExpirationTimestamp());
	}
}

std::vector<TimerQueue::Entry> TimerQueue::GetExpired(Timestamp now_timestamp)
{
	std::vector<Entry> expired;
	Entry border = std::make_pair(now_timestamp, reinterpret_cast<Timer*>(UINTPTR_MAX));
	auto it = timerlist_.lower_bound(border);
	std::copy(timerlist_.begin(), it, back_inserter(expired));
	timerlist_.erase(timerlist_.begin(), it);
	return expired;
}