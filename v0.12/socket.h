#ifndef SOCKET_H
#define SOCKET_H

class InetAddress;

class Socket
{
public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    ~Socket();
    
    int GetSockfd() const { return sockfd_; }
    void Bind(const InetAddress& addr);
    void Listen();
    int Accept(InetAddress* peer_addr);
    void ShutdownWrite();
    
    void SetReuseAddr(bool on);
    void SetTcpNoDelay(bool on);
    void SetKeepAlive(bool on);
private:
    const int sockfd_;
};

#endif // SOCKET_H