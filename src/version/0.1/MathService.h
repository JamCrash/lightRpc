
#ifndef TINY_RPC_MATH_SERVICE_H
#define TINY_RPC_MATH_SERVICE_H

#include "Service.h"
#include "MathServiceStub.h"

#include <unordered_map>

class MathService: public Service, public MathServiceStub {
public:
    ~MathService();

    // TODO: buffer引用方式 2020年07月12日20:17:13
    void do_req(std::string&& inputBuffer);
    void send_resp(std::string&& outputBuffer);

private:
    int add(int x, int y);
    int sub(int x, int y);

private:
    std::unordered_map<int, void*> methods_;
};

#endif //
