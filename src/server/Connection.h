
#ifndef TINY_PRC_CONNECTION_H
#define TINY_PRC_CONNECTION_H

#include <memory>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

class ServiceManager;

class Connection: public std::enable_shared_from_this<Connection>
{
    using ServiceManagerPtr = std::shared_ptr<ServiceManager>;

public:
    Connection(tcp::socket socket, ServiceManagerPtr serviceManager);
    
    void start();

    void stop();

private:
    void do_read_pkg_length();
    void do_read_pkg();
    void do_write(std::string response);

private:
    tcp::socket socket_;
    const static size_t BUFFER_SIZE = 4096;
    char buffer_[BUFFER_SIZE];
    uint32_t pkgLen_; // 请求包长度
    ServiceManagerPtr serviceManager_;
};

#endif //
