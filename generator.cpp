
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

ofstream 

bool generate_method(const Value& method)
{
    
}

bool generate(Document& doc)
{
    if(doc.HasMember("Service") && doc["Service"].IsString()) 
    {
        const char* srvName = doc["Service"].GetString();
        memcpy(serviceName, srvName, strlen(srvName));
        // TODO: 2020年07月19日17:44:48 
        // 接下来生成各个文件
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
    size_t cur = 0; // json 数组指针
    Document doc;   // JSON -->> DOM

    if(!in.is_open()) 
    {
        printf("open file \"%s\" failed\n", argv[1]);
        return 1;
    }

    // 读取json文件
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