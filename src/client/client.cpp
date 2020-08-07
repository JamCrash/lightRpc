
#include <memory>
#include <cstdio>

#include "RpcClient.h"
#include "ClientStub.h"
#include "MathClientStub.h"
#include "../param/MathParam.h"

int main()
{
    RpcClient rpc("3000");
    std::shared_ptr<ClientStub> mathStub(new MathClientStub);
    rpc.regist("Math", mathStub);
    add_req req;
    add_resp resp;
    rpc.Call("Math", "add", req, resp);
    if(resp.err_code != 0) {
        printf("error\n");
        return 0;
    }
    printf("ret = %d\n", resp.ret);
    return 0;
}