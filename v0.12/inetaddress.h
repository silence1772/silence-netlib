#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <netinet/in.h> //INADDR_ANY
#include <string>

class InetAddress
{
public:
    // 限定显式调用
    explicit InetAddress(uint16_t port);
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(const struct sockaddr_in& addr) : addr_(addr) {}

    std::string ToHostPort() const;

    const struct sockaddr_in& GetSockAddrInet() const { return addr_; }
    void SetSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }
private:
    struct sockaddr_in addr_;
};

#endif // INETADDRESS_H