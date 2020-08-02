
#include <rapidjson/document.h>

#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <utility>

using namespace std;
using namespace rapidjson;

#define BUFFERSIZE 4096
#define JSONSIZE 8192
#define GENERALSIZE 1024

enum FILE_TYPE 
{
    ServiceHeader = 1,
    ServiceCpp,
    ServiceWrapper,
    ServiceMethodImpl,
    ServiceParam
};

using k_v = std::pair<std::string, std::string>;

struct Method 
{
    std::string mName;  // method name
    std::vector<k_v> req;
    std::vector<k_v> resp;
};

std::vector<Method> methods;

char serviceName[GENERALSIZE];
//char methodName[GENERALSIZE];
char serviceHeader[GENERALSIZE];
char serviceCpp[GENERALSIZE];
char serviceWrapper[GENERALSIZE];
char serviceMethod[GENERALSIZE];
char serviceParam[GENERALSIZE];

ofstream oSrvHeader;
ofstream oSrvCpp;
ofstream oSrvWrapper;
ofstream oSrvMethod;
ofstream oSrvParam;

bool generate_header();
bool generate_cpp();
bool generate_wrapper();
bool generate_method();
bool generate_param();

std::string conv2upper(char* str)
{
    std::string ret;
    for(int i = 0; i < strlen(str); ++i)
    {
        char ch = str[i];
        if(ch>='a' && ch<='z') ch -= 32;
        ret.push_back(ch);
    }
    return ret;
}

bool generate_method(const Value& method)
{
    if(!method.IsObject())
    {
        return false;
    }

    Method newMethod;
    auto f = [&](const char* key, std::vector<k_v>& vec) 
    {
        for(auto& m: method[key].GetObject())
        {
            if(!m.value.IsString()) 
                return false;
            // TODO: check m.value is legal or not
            vec.push_back({m.name.GetString(), m.value.GetString()});
        }
        return true;
    };

    if(!method.HasMember("method") || !method.HasMember("req") || !method.HasMember("resp"))
        return false;
    
    if(!method["method"].IsString() || !method["req"].IsObject() || !method["resp"].IsObject())
        return false;
    
    newMethod.mName = method["method"].GetString();
    if(!f("req", newMethod.req)) return false;
    if(!f("resp", newMethod.resp)) return false;
    methods.push_back(newMethod);

    return true;
}

bool generate_and_create_file(char* dst, enum FILE_TYPE file_type)
{
    assert(dst != nullptr);
    size_t srvNameLen = strlen(serviceName);
    memcpy(dst, serviceName, srvNameLen);
    assert(srvNameLen < 512);
    char* fileName = dst;
    dst += srvNameLen;

    switch (file_type)
    {
    case ServiceHeader:
        memcpy(dst, "Service.h", strlen("Service.h")+1);
        if(!generate_header()) 
            return false;
        break;
    
    case ServiceCpp:
        memcpy(dst, "Service.cpp", strlen("Service.cpp")+1);
        if(!generate_cpp()) 
            return false;
        break;

    case ServiceWrapper:
        memcpy(dst, "Wrapper.cpp", strlen("Wrapper.cpp")+1);
        if(!generate_wrapper())
            return false;
        break;
    
    case ServiceMethodImpl:
        memcpy(dst, "Method.impl.cpp", strlen("Method.impl.cpp")+1);
        if(!generate_method())
            return false;
        break;

    case ServiceParam:
        memcpy(dst, "Param.h", strlen("Param.h")+1);
        if(!generate_param())
            return false;
        break;

    default:
        printf("error: line %d, unknown file type to generate\n", __LINE__);
        return false;
    }

    return true;
}

bool generate(Document& doc)
{
    if(doc.HasMember("Rpc") && doc["Rpc"].IsArray())
    {
        const Value& methods = doc["Rpc"];
        for(SizeType i=0; i<methods.Size(); ++i) 
        {
            if(!generate_method(methods[i]))
            {
                printf("error: generate service method faild\n");
                return false;
            }
        }
    }
    else
    {
        printf("error: JSON text doesn't have Rpc definition\n");
        return false;
    }

    if(doc.HasMember("Service") && doc["Service"].IsString()) 
    {
        const char* srvName = doc["Service"].GetString();
        memcpy(serviceName, srvName, strlen(srvName)+1); // +1: 把'\0'也加进去

        // 接下来生成各个文件 2020年07月23日01:29:49
        if(!(
            generate_and_create_file(serviceHeader, ServiceHeader)  &&
            generate_and_create_file(serviceCpp, ServiceCpp)   &&
            generate_and_create_file(serviceWrapper, ServiceWrapper)   &&
            generate_and_create_file(serviceMethod, ServiceMethodImpl)    &&
            generate_and_create_file(serviceParam, ServiceParam)
        ))
        {
            printf("error: generate file failed\n");
            return false;
        }
    }
    else
    {
        printf("error: JSON text doesn't have Service name definition\n");
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("usage: generator /filename/\n");
        return 1;
    }

    ifstream in(argv[1]);
    char buffer[BUFFERSIZE];
    char json[JSONSIZE];
    size_t cur = 0; // JSON 数组指针
    Document doc;   // JSON -->> DOM

    if(!in.is_open()) 
    {
        printf("open file \"%s\" failed\n", argv[1]);
        return 1;
    }

    // 读取json(idl)文件
    while(!in.eof()) 
    {
        in.getline(buffer, BUFFERSIZE);
        size_t size = strlen(buffer);
        if(size + cur > JSONSIZE) 
        {
            printf("error: json file too large\n");
            return 1;
        }
        memcpy(json+cur, buffer, size);
        cur += size;
    }
#ifdef TEST
    printf("%s\n", json);
#endif 

    if(doc.ParseInsitu(json).HasParseError()) 
    {
        printf("error: parse json failed\n");
        return 1;
    }

    if(!generate(doc)) 
    {
        printf("error: generate code failed\n");
        return 1;
    }

    printf("generate code success\n");
    return 0;
}

bool generate_header()
{
    auto& o = oSrvHeader;
    o.open(serviceHeader);
    if(o.fail())
    {
        printf("error: create service header file failed\n");
        return false;
    }

    o << "\n#ifndef TINY_RPC_" << conv2upper(serviceName) << "_SERVICE_H\n";
    o << "#define TINY_RPC_" << conv2upper(serviceName) << "_SERVICE_H\n";

    o << "\n#include <unordered_map>\n";
    o << "\n#include \"Service.h\"\n";
    o << "#include \"" << serviceName << "Param.h\"\n";

    o << "\nclass " << serviceName << "Service: public Service\n";
    o << "{\n";

    // 结构体
    o << "public:\n";
    o << "\t" << serviceName << "Service();\n";
    o << "\nprivate:\n\t// methods\n";
    // methods 接口定义
    for(Method& method: methods)
    {
        o << "\tstatic void " << method.mName 
          << "(" << method.mName << "_req& req, "
          << method.mName << "_resp& resp);\n";
    }
    o << "\nprivate:\n";
    o << "\t/// @brief method_wrapper函数解析请求参数,调用method并序列化返回结果\n";
    for(Method& method: methods)
    {
        o << "\tstatic std::string " << method.mName << "_wrapper"
          << "(char* param_data, size_t len)\n";
    }


    o << "};\n";

    o << "\n#endif // \n";

    o.close();
    return true;
}

bool generate_cpp()
{
    auto& o = oSrvCpp;
    o.open(serviceCpp);
    if(o.fail())
    {
        printf("error: create service definition file failed\n");
        // TODO: delete the created file
        return false;
    }

    o << "\n#include \"" << serviceName << "Service.h\"\n";
    o << "#include \"" << serviceName << "Param.h\"\n";

    o << "\n" << serviceName << "Service::" << serviceName << "Service(): "
      << "Service(\"" << serviceName << "\")\n";
    o << "{\n";

    o << "\tauto hashObj = std::hash<std::string>()\n\n";
    for(Method& method: methods)
    {
        o << "\tmethods_[hashObj(\"" << method.mName << "\")] = "
          << method.mName << "_wrapper;\n";
    }

    o << "}\n";

    o.close();
    return true;
}

bool generate_wrapper()
{
    auto& o = oSrvWrapper;
    o.open(serviceWrapper);
    if(o.fail())
    {
        printf("error: create service wrapper file failed\n");
        // TODO: delete the created files
        return false;
    }

    o << "\n#include <cstring>\n";
    o << "#include <rapidjson/document.h>\n";
    o << "#include <rapidjson/prettywriter.h>\n";
    o << "\n#include \"" << serviceName << "Service.h\"\n";
    o << "#include \"LOG.h\"\n";

    for(Method& method: methods)
    {
        o << "\nstd::string " << serviceName << "Service::"
          << method.mName << "_wrapper(char* param_data, size_t len)\n";
        o << "{\n";

        o << "\t" << method.mName << "_req req;\n";
        o << "\t" << method.mName << "_resp resp;\n";
        o << "\tuint32_t pkg_len;\n";
        o << "\tuint32_t ec;\t// 错误标识 0未出错 -1出错\n";
        o << "\trapidjson::Document doc_req;\t// 存储DOM格式请求参数\n";
        o << "\trapidjson::Document doc_resp;\t// 存储DOM格式响应参数\n";
        o << "\tconst char* ret_str = nullptr;\t// 指向序列化后的响应参数\n";
        o << "\trapidjson::StringBuffer sb;\n";
        o << "\trapidjson::PrettyWriter<rapidjson::StringBuffer> writter(sb);\t// 格式化\n";

        o << "\n\tif(doc_req.ParseInsitu(param_data).HasParseError())\n";
        o << "\t{\n";
        o << "\t\tLOG(\"add_wraper: parse request failed\");\n";
        o << "\t}\n\n";

        auto f = [](std::string& desc)
        {
            std::string ret = "Get";
            if(desc == "uint32_t")
            {
                ret += "Uint";
            }
            else if(desc == "int32_t")
            {
                ret += "Int";
            }
            else if(desc == "string")
            {
                ret += "String";
            }
            else 
            {
                ret = "";
            }
            return ret;
        };
        for(int i = 0; i < method.req.size(); ++i) 
        {
            o << "\treq." << method.req[i].first << " = doc_req[\""
              << method.req[i].first << "\"]." << f(method.req[i].second)+"();\n";
        }

        o << "\n\t" << method.mName << "(req, resp);\n";

        o << "\t// 序列化返回参数\n";
        o << "\tdoc_resp.Parse(\"{}\");\n";
        for(int i = 0; i < method.resp.size(); ++i)
        {
            o << "\tdoc_resp.AddMember(\"" << method.resp[i].first << "\", resp."
              << method.resp[i].first << ", doc_resp.GetAllocator());\n";
        }
        o << "\n\tdoc_resp.Accept(writter);\n";
        o << "\tret_str = sb.GetString();\n";

        o << "\n\tpkg_len = 4 + strlen(ret_str);\n";
        o << "\tec = 0;\n";
        o << "\tstd::string s1((char*)&(pkg_len), 4);\n";
        o << "\tstd::string s2((char*)&(ec), 4);\n";

        o << "\n\treturn s1 + s2 + std::string(ret_str, strlen(ret_str));\n";

        o << "}\n";
    }

    o.close();
    return true;
}

bool generate_method()
{
    auto& o = oSrvMethod;
    o.open(serviceMethod);
    if(o.fail())
    {
        printf("error: create method.impl file failed\n");
        // TODO: delete the created files
        return false;
    }

    o << "\n#include \"" << serviceName << "Service.h\"\n";

    for(Method& method: methods)
    {
        o << "\nvoid " << serviceName << "Service::" << method.mName
          << "(" << method.mName << "_req& req, "
          << method.mName << "_resp& resp)\n";
        o << "{\n";
        o << "\n}\n";
    }

    o.close();
    return true;
}

bool generate_param()
{
    auto& o = oSrvParam;
    o.open(serviceParam);
    if(o.fail())
    {
        printf("error: create parameter header file failed\n");
        // TODO: delete the created files
        return false;
    }

    o << "\n#ifndef TINY_RPC_" << conv2upper(serviceName) << "_PARAM_H\n";
    o << "#define TINY_RPC_" << conv2upper(serviceName) << "_PARAM_H\n";

    o << "\n#include <cstdint>\n";

    for(Method& method: methods)
    {
        // request struct 
        o << "\nstruct " << method.mName << "_req\n";
        o << "{\n";
        for(int i = 0; i < method.req.size(); ++i)
        {
            o << "\t" << method.req[i].second << " " 
              << method.req[i].first << ";\n";
        }
        o << "};\n";

        // response struct
        o << "\nstruct " << method.mName << "_resp\n";
        o << "{\n";
        for(int i = 0; i < method.resp.size(); ++i)
        {
            o << "\t" << method.resp[i].second << " "
              << method.resp[i].first << ";\n";
        }
        o << "};\n";
    }

    o << "\n#endif // \n";    // define header

    o.close();
    return true;
}
