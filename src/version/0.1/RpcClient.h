
#ifndef TINY_RPC_CLIENT_H
#define TINY_RPC_CLIENT_H

#include <memory>
#include <unordered_map>
#include <string>

class ClientService;

class RpcClient {
public:

    template<typename... Args>
    void Call(std::string serviceName, std::string methodName, Args... args);

private:
    std::unordered_map<int, std::shared_ptr<ClientService>> services_;
};

#endif // 
