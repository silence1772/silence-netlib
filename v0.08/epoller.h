#ifndef EPOLLER_H
#define EPOLLER_H

#include <vector>
#include <map>

// 前向声明
struct epoll_event;
class EventItem;
class EventLoop;

class Epoller
{
public:
	typedef std::vector<EventItem*> EventItemList;

	Epoller();
	~Epoller();
	
	int Poll(EventItemList* active_eventitems);
	void UpdateEventItem(EventItem* eventitem);

private:
	void Update(int operation, EventItem* eventitem);
	void FillActiveEventItems(int num_events, EventItemList* active_eventitems) const;
	
	typedef std::map<int, EventItem*> EventItemMap;
	typedef std::vector<struct epoll_event> EventList;

	EventItemMap eventitems_;
	EventList events_;

	int epollfd_;
};

#endif // EPOLL_H