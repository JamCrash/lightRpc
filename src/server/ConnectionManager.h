
#ifndef TINY_RPC_CONNETION_MANAGER_H
#define TINY_RPC_CONNETION_MANAGER_H

#include <memory>
#include <set>
#include <asio.hpp>

class Connection;

class ConnectionManager 
{
    using ConnectionPtr = std::shared_ptr<Connection>;

public:
    void start(ConnectionPtr newConn);

    void stop(ConnectionPtr conn);

    void stop_all();

private:
    std::set<ConnectionPtr> connections_;
};

#endif //
