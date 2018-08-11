#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <vector>
#include <memory>
#include "currentthread.h"

class EventItem;
class Epoller;
class TimerQueue;
class Timestamp;

class EventLoop
{
public:
	EventLoop();
	~EventLoop();
	
	typedef std::function<void()> TimerCallback;

	void Loop();
	void Quit() { quit_ = true; }
	void UpdateEventItem(EventItem* eventitem);

	bool IsInBirthThread() const { return thread_id_ == GetTid(); }
	void AssertInBirthThread();

	void RunAt(const Timestamp& timestamp, const TimerCallback& cb);
	void RunAfter(double delay, const TimerCallback& cb);
	void RunEvery(double interval, const TimerCallback& cb);
private:
	void Abort();

	typedef std::vector<EventItem*> EventItemList;

	bool quit_;
	const pid_t thread_id_;
	EventItemList active_eventitems_;
	std::unique_ptr<Epoller> epoller_;
	std::unique_ptr<TimerQueue> timer_queue_;
};

#endif // EVENTLOOP_H
