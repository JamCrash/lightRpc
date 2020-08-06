
#ifndef TINY_RPC_MATH_SERVICE_H
#define TINY_RPC_MATH_SERVICE_H

#include <unordered_map>

#include "Service.h"
#include "MathParam.h"

class MathService: public Service
{
public:
	MathService();

private:
	// methods
	static void add(add_req& req, add_resp& resp);
	static void sub(sub_req& req, sub_resp& resp);

private:
	/// @brief method_wrapper函数解析请求参数,调用method并序列化返回结果
	static std::string add_wrapper(char* param_data, size_t len);
	static std::string sub_wrapper(char* param_data, size_t len);
};

#endif // 
