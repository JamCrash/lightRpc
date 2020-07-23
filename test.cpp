
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

using namespace std;
using namespace rapidjson;

int main(int argc, char* argv[])
{
    if(argc != 2) return 0;
    ifstream input;
    input.open(argv[1]);
    char buffer[2048];
    input.read(buffer, 2048);
    cout << strlen(buffer) << endl;
    Document doc;
    if(doc.ParseInsitu(buffer).HasParseError()) 
        return 0;
    StringBuffer sb;
    PrettyWriter<StringBuffer> pw(sb);
    doc.Accept(pw);
    cout << strlen(sb.GetString()) << endl;
}
