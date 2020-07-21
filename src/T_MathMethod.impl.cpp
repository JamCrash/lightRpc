
/*

此文件由框架生成,用户实现具体逻辑

*/

#include "T_MathService.h"

void MathService::add(add_req& req, add_resp& resp)
{
    resp.err_code = 0;
    resp.ret = req.x + req.y;
}
