
#include <stdio.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace rapidjson;

ofstream out;

int main()
{
    out.open("file");
    if(out.fail()) {
        cout << "fuck\n";return -1;
    }
    out << "hhhh\n";
    return 0;
}  
