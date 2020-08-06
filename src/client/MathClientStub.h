
#ifndef TINY_RPC_MATH_CLIENT_STUB_H
#define TINY_RPC_MATH_CLIENT_STUB_H

#include "ClientStub.h"

class MathClientStub: public ClientStub
{
public:
    MathClientStub();
    std::string marshal(void* req, size_t len, std::string& methodName) override;

private:
    std::string marshal_add(void* req, size_t len);
    std::string marshal_sub(void* req, size_t len);
};

#endif //
