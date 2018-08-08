#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

class EventLoop;

class Channel
{
public:
    // 回调函数类型
    typedef std::function<void()> EventCallback;

    // 构造函数
    Channel(EventLoop* loop, int fd);
    // 析构函数
    ~Channel();
    
    // 处理事件
    void HandleEvent();
    // 设置读事件回调
    void SetReadCallback(const EventCallback& cb) { read_callback_ = cb; }

    void EnableReading();

    int GetFd() const { return fd_; }
    int GetEvents() const { return events_; }
    void SetRevents(int revents) { revents_ = revents; }
    bool IsNoneEvent() const { return events_ == 0; }

    int GetStatus() { return status_; }
    void SetStatus(int status) { status_ = status; }


private:
    // 更新channel
    void Update();

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int status_;

    EventCallback read_callback_;
};

#endif // CHANNEL_H