
#ifndef TINY_RPC_CLIENT_H
#define TINY_RPC_CLIENT_H

#include <asio.hpp>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <memory>

#include "ClientStub.h"

using asio::ip::tcp;

class RpcClient
{
public:
	RpcClient(std::string hostname, std::string port);
	RpcClient(std::string port);
	~RpcClient();

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

#endif //
