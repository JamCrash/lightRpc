
#include "RpcServer.h"
#include "Service.h"
#include "T_MathService.h"
#include <memory>
using namespace std;

int main()
{
    shared_ptr<Service> srv = make_shared<MathService>();
    RpcServer server;
    server.registService(srv);
    server.run();
}
