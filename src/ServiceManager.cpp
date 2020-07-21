
#include "ServiceManager.h"
#include "Service.h"

#include "Log.h"

void ServiceManager::registerService(ServicePtr newService)
{
    std::string srvName = newService->get_service_name();
    size_t hVal = std::hash<std::string>()(srvName);

    if(services_.count(hVal) != 0) 
    {
        // TODO: 打LOG 2020年07月16日23:57:30
        std::cout << "service: " << srvName << " exist!" << std::endl;
        return ;
    }

    services_[hVal] = newService;
}

std::string ServiceManager::dispatch(char* rawData, size_t len)
{
    uint64_t sid;   // serviceId
    ServicePtr service;
    std::string pkg(rawData, len);
    std::string resp;   // 最终结果

    if(!parseServiceId(&sid, rawData))
    {
        // 未知错误
        // TODO: 返回错误(记得打LOG) 2020年07月16日20:24:46
        LOG_TEST("error dispath: parseServiceId");
        // return ;
    }

    // find service
    auto it = services_.find(sid);
    if(it == services_.end())
    {
        // 服务不存在
        // TODO: 返回错误(记得打LOG) 2020年07月16日20:04:57
        LOG_TEST("error dispath: service not found");
        // return ;
    }
    service = it->second;

    return service->handle_req(rawData+8, len-8);
}

bool ServiceManager::parseServiceId(uint64_t* sid, const char* rawData)
{
    if(sid == nullptr || rawData == nullptr) return false;

    *sid = *reinterpret_cast<uint64_t*>(const_cast<char*>(rawData));
    return true;
}
