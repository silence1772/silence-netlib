#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <stdint.h>
#include <set>
#include <vector>
#include <functional>
#include "eventitem.h"

class EventLoop;
class Timer;
class Timestamp;

class TimerQueue
{
public:
	TimerQueue(EventLoop* loop);
	~TimerQueue();

	typedef std::function<void()> TimerCallback;
	
	void AddTimer(const TimerCallback& cb, Timestamp timestamp, double interval = 0.0);

private:
	typedef std::pair<Timestamp, Timer*> Entry;
	typedef std::set<Entry> TimerList;

	void HandelTimerExpired();
	std::vector<Entry> GetExpired(Timestamp now_timestamp);

	const int timerfd_;
	EventItem timerfd_eventitem_;
	TimerList timerlist_;
	EventLoop* loop_;
};

#endif // TIMERQUEUE_H