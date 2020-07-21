
#ifndef TINY_RPC_SERVICE_H
#define TINY_RPC_SERVICE_H

#include <functional>
#include <unordered_map>
#include <string>

class Service
{
    using wraperFunc = std::function<std::string(char*, size_t)>;

public:
    Service(std::string srvName): serviceName_(srvName) {}
    
    /// @brief
    /// 分析请求内容,解析出调用的methodName,组装请求参数并调用对应的method,返回序列化后的结果
    /// rawReq: methodName + "\n" + 请求参数
    /// len: rawReq长度
    std::string handle_req(char* rawReq, size_t len);

    std::string get_service_name() const
    { return serviceName_; }

protected:

    std::string parse_method(char* rawReq, size_t len);

protected:
    std::string serviceName_;
    std::unordered_map<uint64_t, wraperFunc> methods_;
};

#endif //
