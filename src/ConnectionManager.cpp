
#include "ConnectionManager.h"
#include "Connection.h"

void ConnectionManager::start(ConnectionPtr newConn)
{
    connections_.insert(newConn);
    newConn->start();
}

void ConnectionManager::stop(ConnectionPtr conn)
{
    connections_.erase(conn);
    conn->stop();
}

void ConnectionManager::stop_all()
{
    for(auto connPtr: connections_)
    {
        connPtr->stop();
    }
    connections_.clear();
}
