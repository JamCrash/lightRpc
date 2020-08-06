
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

template<typename ReqT, typename RespT>
void RpcClient::Call(std::string serviceName, std::string methodName, ReqT& req, RespT& resp)
{
    auto sid = static_cast<uint64_t>(hash(serviceName));
    auto it = stubs_.find(sid);
    if(it == stubs_.end())
    {
        // TODO: service not found
        // 2020年08月06日21:21:31
        return ;
    }

    std::string ret = it->second->marshal((void*)&req, sizeof(ReqT), methodName);
    if(ret == "") 
    {
        // TODO: marshal error
        // 2020年08月06日21:23:24
        return ;
    }

    uint32_t pkgLen = 8 + methodName.size() + 1 + ret.size();
    asio::write(socket_, asio::buffer(&pkgLen, 4));
    asio::write(socket_, asio::buffer(&sid, 8));
    asio::write(socket_, asio::buffer((methodName+"\n").c_str(), methodName.size()+1));
    asio::write(socket_, asio::buffer(ret.c_str(), ret.size()));

    
}

void RpcClient::regist(std::string srvName, std::shared_ptr<ClientStub> stub)
{
    stubs_[hashObj_(srvName)] = stub;
}