
#ifndef TINY_RPC_CLIENT_STUB_H
#define TINY_RPC_CLIENT_STUB_H

#include <string>
#include <functional>
#include <unordered_map>

class ClientStub
{
    using marshalFunc = std::function<std::string(void*, size_t)>;
public:
    virtual std::string marshal(void* req, size_t len, std::string& methodName) = 0;
    //virtual bool unmarshal()
private: 
    std::unordered_map<size_t, marshalFunc> funcs_;
};

#endif //
