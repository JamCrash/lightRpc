
#include "Service.h"

#include "Log.h"

std::string Service::handle_req(char* rawReq, size_t len)
{
    std::string method = parse_method(rawReq, len);
    uint64_t hVal;
    
    if(method.size() == 0)
    {
        // parse method error
        // TODO: LOG 2020年07月17日23:49:14
        LOG("handle_req");
        // return error msg;
    }

    hVal = std::hash<std::string>()(method);
    auto it = methods_.find(hVal);
    if(it == methods_.end()) 
    {
        // method not found
        // TODO: LOG 2020年07月18日00:35:56
        LOG("handle_req");
        // return error msg;
    }

    // +1/-1 是为了略过换行符
    return it->second(rawReq+1+method.size(), len-1-method.size());
}

std::string Service::parse_method(char* rawReq, size_t len)
{
    size_t i;

    for(i=0; i<len&&(*(rawReq+i)!='\n'); ++i);

    if(i == len) return std::string();

    return std::string(rawReq, i);
}
