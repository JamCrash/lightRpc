
#ifndef LOG_H
#define LOG_H

#include <iostream>

#define LOG(x) (std::cout << x << std::endl)

#ifdef TEST
#define LOG_TEST(x) (std::cout << x << std::endl)
#else
#define LOG_TEST(x)
#endif 

#endif //