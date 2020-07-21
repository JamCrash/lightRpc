
#ifndef TINY_RPC_SERVICE_STUB_H
#define TINY_RPC_SERVICE_STUB_H

class ServiceStub {
public:
    // TODO: 加上inputbuffer、outputbuffer 2020年07月12日21:22:14
    virtual void serialize_req() = 0;
    virtual void serialize_resp() = 0;
    virtual void unserialize_req() = 0;
    virtual void unserialize_resp() = 0;
};

#endif // 
