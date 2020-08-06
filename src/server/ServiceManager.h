
#ifndef TINY_RPC_SERVICE_MANAGER_H
#define TINY_RPC_SERVICE_MANAGER_H

#include <unordered_map>
#include <string>
#include <memory>

class Service;

class ServiceManager
{
    using ServicePtr = std::shared_ptr<Service>;

public:
    void registerService(ServicePtr newService);
    
    /// @brief
    /// 向各个service分发请求并返回序列化后的结果
    /// rawData: 8个字节serviceId + methodName + "\n" + 请求参数
    /// len: rawData长度
    /// 
    /// @n rawData中数据均为原生表示,即在发送时不进行任何类型转换(例如不将数字转为字符串再传输)
    std::string dispatch(char* rawData, size_t len);

private:
    /// true if parse success, false if parse error
    bool parseServiceId(uint64_t* sid, const char* rawData);

private:
    // TODO: uint64_t 合适吗 2020年07月16日01:13:02
    std::unordered_map<uint64_t, ServicePtr> services_;
};

#endif //
