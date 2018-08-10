#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <vector>
#include <memory>

class EventItem;
class Epoller;

class EventLoop
{
public:
	EventLoop();
	~EventLoop();
	
	void Loop();
	void Quit() { quit_ = true; }
	void UpdateEventItem(EventItem* eventitem);

private:
	typedef std::vector<EventItem*> EventItemList;

	bool quit_;
	EventItemList active_eventitems_;
	std::unique_ptr<Epoller> epoller_;
};

#endif // EVENTLOOP_H
