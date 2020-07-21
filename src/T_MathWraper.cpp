
/*

此文件应由脚本生成

*/

#include <cstring>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include "T_MathService.h"

#include "Log.h"

std::string MathService::add_wraper(char* param_data, size_t len)
{
    add_req req;
    add_resp resp;
    uint32_t pkg_len;
    uint32_t ec;    // 错误标识 0未出错 -1出错
    std::string ret;
    rapidjson::Document doc_req;    // 存储DOM格式请求参数
    rapidjson::Document doc_resp;   // 存储DOM格式响应参数
    //rapidjson::StringBuffer sb;
    //rapidjson::PrettyWriter<rapidjson::StringBuffer> writter;   // 格式化

    /*
    if(len != sizeof(add_req))
    {
        // parse param error
        // TODO: LOG 2020年07月18日12:03:30
        LOG("add_wraper");
        // return ;
    }
    std::memcpy((void*)&req, (const void*)param_data, len);
    */

    if(!doc_req.ParseInsitu(param_data).HasParseError())
    {
        // parse param error
        // TODO: LOG 2020年07月21日16:56:09
        LOG("add_wraper: parse request failed");
        // return ;
    }

    req.x = doc_req["x"].GetInt();
    req.y = doc_req["y"].GetInt();

    add(req, resp);

    // 序列化返回参数


    pkg_len = 4 + sizeof(resp);
    ec = 0;
    std::string s1((char*)&(pkg_len), 4);
    std::string s2((char*)&(ec), 4);



    return s1 + s2 + std::string(reinterpret_cast<const char*>(&resp), sizeof(resp));
}
