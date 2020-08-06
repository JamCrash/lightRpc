
#include "MathService.h"
#include "MathParam.h"

MathService::MathService(): Service("Math")
{
	auto hashObj = std::hash<std::string>();

	methods_[hashObj("add")] = add_wrapper;
	methods_[hashObj("sub")] = sub_wrapper;
}
