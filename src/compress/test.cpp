
#include <functional>
#include <iostream>
using namespace std;

int main()
{
    auto f = [&](int a) {
        if(a == 1) return a;
        return f(a-1);
    };
    cout << f(5) << endl;
}