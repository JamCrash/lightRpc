
#include "MathService.h"

void MathService::add(add_req& req, add_resp& resp)
{
  resp.err_code = 0;
  resp.ret = req.x + req.y;
}

void MathService::sub(sub_req& req, sub_resp& resp)
{
  resp.err_code  = 0;
  resp.ret = req.x - req.y;
}
