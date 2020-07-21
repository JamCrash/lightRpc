
#ifndef TINY_RPC_SERVICE_H
#define TINY_RPC_SERVICE_H

#include <string>
#include <memory>

class RpcServer;

class Service {
public:

    // TODO: buffer的引用方式？(右值or左值?) 2020年07月12日20:07:50
    virtual void do_req(std::string&& inputBuffer) = 0;
    virtual void send_resp(std::string&& outputBuffer) = 0;

private:
    std::shared_ptr<RpcServer> server_;
};

#endif //
