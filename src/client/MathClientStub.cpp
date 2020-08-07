
#include <asio.hpp>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <algorithm>

#include "MathClientStub.h"
#include "../param/MathParam.h"

using namespace rapidjson;
using namespace std::placeholders;

MathClientStub::MathClientStub()
{
    auto hashObj = std::hash<std::string>();
    marshalFuncs_[hashObj("add")] = std::bind(&MathClientStub::marshal_add, this, _1, _2);
    marshalFuncs_[hashObj("sub")] = std::bind(&MathClientStub::marshal_sub, this, _1, _2);

    unmarshalFuncs_[hashObj("add")] = std::bind(&MathClientStub::unmarshal_add, this, _1, _2, _3);
}

std::string MathClientStub::marshal(void* req, size_t len, std::string& methodName)
{
    auto hashObj = std::hash<std::string>();
    auto it = marshalFuncs_.find(hashObj(methodName));
    if(it == marshalFuncs_.end())
    {
        // TODO: method not found
        // 2020年08月06日20:41:54
        return "";
    }
    return it->second(req, len);
}

bool MathClientStub::unmarshal(void* resp, size_t len, std::string& methodName, Document& doc)
{
    auto hashObj = std::hash<std::string>();
    auto it = unmarshalFuncs_.find(hashObj(methodName));
    if(it == unmarshalFuncs_.end()) 
    {
        // TODO: method not found
        // 2020年08月07日22:02:57
        return "";
    }
    return it->second(resp, len, doc);
}

std::string marshal_add(void* reqPtr, size_t len)
{
    auto req = static_cast<add_req*>(reqPtr);
    Document doc;
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    doc.Parse("{}");
    doc.AddMember("x", req->x, doc.GetAllocator());
    doc.AddMember("y", req->y, doc.GetAllocator());
    doc.Accept(writer);
    return sb.GetString();
}

std::string marshal_sub(void* reqPtr, size_t len)
{

}

bool unmarshal_add(void* respPtr, size_t len, Document& doc)
{
    auto resp = static_cast<add_resp*>(respPtr);
    resp->err_code = doc["err_code"].GetInt();
    resp->ret = doc["ret"].GetInt();
    return true;
}
