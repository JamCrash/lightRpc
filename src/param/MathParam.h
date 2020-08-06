
#ifndef TINY_RPC_MATH_PARAM_H
#define TINY_RPC_MATH_PARAM_H

#include <cstdint>

struct add_req
{
	int32_t x;
	int32_t y;
};

struct add_resp
{
	int32_t err_code;
	int32_t ret;
};

struct sub_req
{
	int32_t x;
	int32_t y;
};

struct sub_resp
{
	int32_t err_code;
	int32_t ret;
};

#endif // 
