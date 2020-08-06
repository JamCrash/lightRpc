
#ifndef TINY_RPC_RPC_SERVER_H
#define TINY_RPC_RPC_SERVER_H

#include <memory>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

class ConnectionManager;
class ServiceManager;
class Service;

class RpcServer 
{
public:
    RpcServer();
    ~RpcServer();

    void run();

    void registService(std::shared_ptr<Service> service);
    
private:
    void do_async_wait_signals();
    
    void do_accept();

private:
    asio::io_context ctx_;
    asio::signal_set signals_;
    short port_;
    tcp::acceptor acceptor_;
    std::unique_ptr<ConnectionManager> connectionManager_;
    std::shared_ptr<ServiceManager> serviceManager_;
};

#endif //
