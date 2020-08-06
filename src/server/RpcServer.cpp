
#include "RpcServer.h"
#include "ConnectionManager.h"
#include "Connection.h"
#include "ServiceManager.h"
#include "Service.h"

#include "Log.h"

RpcServer::RpcServer() 
: ctx_(),
  signals_(ctx_),
  port_(3000),
  acceptor_(ctx_, tcp::endpoint(tcp::v4(), port_)),
  connectionManager_(new ConnectionManager),
  serviceManager_(new ServiceManager)
{
    signals_.add(SIGTERM);
    signals_.add(SIGINT);
    do_async_wait_signals();

    do_accept();
}

RpcServer::~RpcServer()
{}

void RpcServer::run()
{
    LOG("server run");

    ctx_.run();

    LOG("server stop");
}

void RpcServer::registService(std::shared_ptr<Service> newService)
{
    serviceManager_->registerService(newService);
}

void RpcServer::do_async_wait_signals()
{
    signals_.async_wait(
        [this](std::error_code /*ec*/, int /*signo*/)
        {
            acceptor_.close();
            connectionManager_->stop_all();
        }
    );
}

void RpcServer::do_accept()
{
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
            if(!acceptor_.is_open()) 
            {
                return ;
            }

            if(!ec)
            {
                connectionManager_->start(std::make_shared<Connection>(
                    std::move(socket),
                    serviceManager_
                ));
            }
            do_accept();
        }
    );
}