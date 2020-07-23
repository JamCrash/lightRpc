
#include <stdio.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <fstream>
#include <stdlib.h>

using namespace std;
using namespace rapidjson;

int main()
{
    const char* json = "{\"a\": 1, \"b\": \"c\"}";
    Document doc;
    if(doc.Parse(json).HasParseError()) return 1;
    for(auto& m: doc.GetObject()) 
    {
        printf("%s\n", m.name.GetString());
    }
    const char* json2 = "{\"a\": [1, 2, \"c\"]}";
    if(doc.Parse(json2).HasParseError()) return 1;
    for(auto& m: doc["a"].GetArray())
    {
        printf("%d\n", m.GetInt());
    }
}
