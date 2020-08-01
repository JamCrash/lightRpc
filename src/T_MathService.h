
// 框架生成

#ifndef MATH
#define MATH

#include <unordered_map>

#include "Service.h"
#include "T_MathParam.h"

class MathService: public Service 
{
public:
    MathService();

private:
    // methods
    static void add(add_req& req, add_resp& resp);

private:
    /// @brief method_wraper函数解析请求参数,调用method并序列化返回结果
    static std::string add_wrapper(char* param_data, size_t len);
};

#endif //
