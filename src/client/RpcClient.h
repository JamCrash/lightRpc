
#ifndef TINY_RPC_CLIENT_H
#define TINY_RPC_CLIENT_H

#include <asio.hpp>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <memory>

#include "ClientStub.h"

#define MAXLEN 10240

using asio::ip::tcp;

class RpcClient
{
public:
	RpcClient(std::string hostname, std::string port);
	RpcClient(std::string port);
	~RpcClient() {}

    /// @brief
    /// 序列化req,打包请求并发送,接受结果并反序列化至resp中
	template<typename ReqT, typename RespT>
	void Call(std::string serviceName, std::string methodName, 
			  ReqT& req, RespT& resp);

    void regist(std::string srvName, std::shared_ptr<ClientStub> stub);

private:
	asio::io_context ctx_;
	tcp::socket socket_;
	tcp::resolver resolver_;

    std::hash<std::string> hashObj_;
    std::unordered_map<uint64_t, std::shared_ptr<ClientStub>> stubs_;
};

template<typename ReqT, typename RespT>
void RpcClient::Call(std::string serviceName, std::string methodName, ReqT& req, RespT& resp)
{
    auto sid = static_cast<uint64_t>(std::hash<std::string>()(serviceName));
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

    char reply[MAXLEN];
    asio::read(socket_, asio::buffer(&pkgLen, 4));
    uint32_t ec;
    asio::read(socket_, asio::buffer(&ec, 4));
    if(ec != 0) {
        // TODO: response error
        // 2020年08月07日21:33:27
        return ;
    }
    asio::read(socket_, asio::buffer(reply, pkgLen - 4));

    Document doc;
    if(doc.ParseInsitu(reply).GetParseError())
    {
        // TODO: parse reponse error
        // 2020年08月07日21:38:06
        return ;
    }
    if(!it->second->unmarshal((void*)&resp, sizeof(RespT), methodName, doc))
    {
        // TODO: unmarshal error
        // 2020年08月07日22:45:30
        return ;
    }
}

#endif //
