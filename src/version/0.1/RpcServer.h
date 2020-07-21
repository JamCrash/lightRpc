
#ifndef TINY_RPC_SERVER_H
#define TINY_RPC_SERVER_H

#include <unordered_map>
#include <memory>
#include <string>
#include "asio.hpp"

using asio::ip::tcp;
class Service;

class RpcServer {
    friend class Service;
public:
    RpcServer();
    ~RpcServer();

    void run();

private:
    void send_resp(std::string&& respData);

private:
    asio::io_context ctx_;
    short port_;
    tcp::acceptor acceptor_;

    std::unordered_map<int, std::shared_ptr<Service>> services_;
};

#endif //
