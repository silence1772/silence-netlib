#ifndef EVENTITEM_H
#define EVENTITEM_H

#include <functional>

class EventLoop;

class EventItem
{
public:
    // 回调函数类型
    typedef std::function<void()> EventCallback;

    // 构造函数
    EventItem(EventLoop* loop, int fd);
    // 析构函数
    ~EventItem();
    
    // 处理事件
    void HandleEvent();
    // 设置读事件回调
    void SetReadCallback(const EventCallback& cb) { read_callback_ = cb; }

    void EnableReading();

    int GetFd() const { return fd_; }
    int GetConcernEventTypes() const { return concern_event_types_; }
    void SetActiveEventTypes(int active_event_types) { active_event_types_ = active_event_types; }
    bool IsNoneEvent() const { return concern_event_types_ == 0; }

    int GetStatus() { return status_; }
    void SetStatus(int status) { status_ = status; }


private:
    // 更新EventItem
    void Update();

    EventLoop* loop_;
    const int fd_;
    int concern_event_types_;
    int active_event_types_;
    int status_;

    EventCallback read_callback_;
};

#endif // EVENTITEM_H