
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

using namespace std;
using namespace rapidjson;

int main(int argc, char* argv[])
{
    Document doc;
    ifstream in("file");
    char buffer[10240];
    while(!in.eof()) {
        in.getline(buffer, 10240);
    }
    if(doc.ParseInsitu(buffer).HasParseError()) {
        cout << "parse failed\n";
        return 0;
    }
    Value& v = doc["han"];
    assert(v.IsObject());
    //cout << v["han"] << endl;
   // v["han"];
   if(v.HasMember("fa")) {
       cout << "Yes" << endl;
       cout << v["fa"].GetInt() << endl;
   }
}
