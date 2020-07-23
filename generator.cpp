
#include <rapidjson/document.h>

#include <cstdio>
#include <fstream>

using namespace std;
using namespace rapidjson;

#define BUFFERSIZE 4096
#define JSONSIZE 8192
#define GENERALSIZE 1024

char serviceName[GENERALSIZE];
char methodName[GENERALSIZE];
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

enum FILE_TYPE {
    ServiceHeader = 1,
    ServiceCpp,
    ServiceWrapper,
    ServiceMethodImpl,
    ServiceParam
};

bool generate_method(const Value& method)
{
    
}

bool generate_and_create_file(char* dst, enum file_type)
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
        oSrvHeader.open(serviceHeader);
        
        break;
    
    case ServiceCpp:
        memcpy(dst, "Service.cpp", strlen("Service.cpp")+1);
        break;

    case ServiceWrapper:
        memcpy(dst, "Wrapper.cpp", strlen("Wrapper.cpp")+1);
        break;
    
    case ServiceMethodImpl:
        memcpy(dst, "Method.impl.cpp", strlen("Method.impl.cpp")+1);
        break;

    case ServiceParam:
        memcpy(dst, "Param.h", strlen("Param.h")+1);
        break;

    default:
        printf("error: line %d, unknown file type to generate\n", __LINE__);
        return false;
    }

    return true;
}

bool generate(Document& doc)
{
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
    }

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