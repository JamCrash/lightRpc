
#ifndef TINY_RPC_CLIENT_STUB_H
#define TINY_RPC_CLIENT_STUB_H

#include <string>
#include <functional>
#include <unordered_map>
#include <rapidjson/document.h>

using namespace rapidjson;

class ClientStub
{
    using marshalFunc = std::function<std::string(void*, size_t)>;
    using unmarshalFunc = std::function<bool(void*, size_t, Document&)>;

public:
    virtual std::string marshal(void* req, size_t len, std::string& methodName) = 0;
    virtual bool unmarshal(void* resp, size_t len, std::string& methodName, Document& doc) = 0;

protected:
    std::unordered_map<size_t, marshalFunc> marshalFuncs_;
    std::unordered_map<size_t, unmarshalFunc> unmarshalFuncs_;
};

#endif //
