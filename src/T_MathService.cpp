
/*

本文件由框架生成

*/

#include "T_MathService.h"
#include "T_MathParam.h"

#include "Log.h"

MathService::MathService(): Service("Math")
{
    auto hashObj = std::hash<std::string>();

    methods_[hashObj("add")] = add_wrapper;
}
