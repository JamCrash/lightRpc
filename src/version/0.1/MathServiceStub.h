
#ifndef TINY_RPC_MATH_SERVICE_STUB_H
#define TINY_RPC_MATH_SERVICE_STUB_H

#include "ServiceStub.h"

class MathServiceStub: public ServiceStub {
public:
    // TODO: 加上buffer 2020年07月12日22:29:50
    void serialize_req();
    void serialize_resp();
    void unserialize_req();
    void unserialize_resp();
};

#endif //
