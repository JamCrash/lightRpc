
#include "RpcServer.h"
#include "Log.h"

RpcServer::RpcServer()
  : ctx_(),
    port_(3000),
    acceptor_(ctx_, tcp::endpoint(tcp::v4(), port_))
    {}

void RpcServer::run()
{
    LOG("server run");

    
    ctx_.run();

    LOG("server end");
}
