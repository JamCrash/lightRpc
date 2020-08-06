
#include <asio.hpp>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <algorithm>

#include "MathClientStub.h"
#include "../param/MathParam.h"

using namespace rapidjson;

MathClientStub::MathClientStub()
{
    auto hashObj = std::hash<std::string>();
    funcs_[hashObj("add")] = &marshal_add;
    funcs_[hashObj("sub")] = &marshal_sub;
}

std::string MathClientStub::marshal(void* req, size_t len, std::string& methodName)
{
    auto hashObj = std::hash<std::string>();
    auto it = funcs_.find(hashObj(methodName));
    if(it == funcs_.end())
    {
        // TODO: method not found
        // 2020年08月06日20:41:54
        return "";
    }
    return it->second(req, len);
}

std::string marshal_add(void* reqPtr, size_t len)
{
    auto req = static_cast<add_req*>(reqPtr);
    Document doc;
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    doc.Parse("{}");
    doc.AddMember("x", req.x, doc.GetAllocator());
    doc.AddMember("y", req.y, doc.GetAllocator());
    doc.Accept(writer);
    return sb.GetString();
}

std::string marshal_sub(void* reqPtr, size_t len)
{

}
