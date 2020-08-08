
#include <cstdio>

#include "RpcClient.h"

RpcClient::RpcClient(std::string hostname, std::string port)
: ctx_(),
  socket_(ctx_),
  resolver_(ctx_),
  hashObj_(std::hash<std::string>())
{
    asio::connect(socket_, resolver_.resolve(hostname, port));
    printf("connect to server --- %s:%s\n", hostname.c_str(), port.c_str());
}

RpcClient::RpcClient(std::string port): RpcClient("127.0.0.1", port)
{ 
}

void RpcClient::regist(std::string srvName, std::shared_ptr<ClientStub> stub)
{
    stubs_[hashObj_(srvName)] = stub;
}
