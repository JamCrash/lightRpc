
#include <cstring>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include "MathService.h"
#include "Log.h"

std::string MathService::add_wrapper(char* param_data, size_t len)
{
	add_req req;
	add_resp resp;
	uint32_t pkg_len;
	uint32_t ec;	// 错误标识 0未出错 -1出错
	rapidjson::Document doc_req;	// 存储DOM格式请求参数
	rapidjson::Document doc_resp;	// 存储DOM格式响应参数
	const char* ret_str = nullptr;	// 指向序列化后的响应参数
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writter(sb);	// 格式化

	if(doc_req.ParseInsitu(param_data).HasParseError())
	{
		LOG("add_wraper: parse request failed");
	}

	req.x = doc_req["x"].GetInt();
	req.y = doc_req["y"].GetInt();

	add(req, resp);
	// 序列化返回参数
	doc_resp.Parse("{}");
	doc_resp.AddMember("err_code", resp.err_code, doc_resp.GetAllocator());
	doc_resp.AddMember("ret", resp.ret, doc_resp.GetAllocator());

	doc_resp.Accept(writter);
	ret_str = sb.GetString();

	pkg_len = 4 + strlen(ret_str);
	ec = 0;
	std::string s1((char*)&(pkg_len), 4);
	std::string s2((char*)&(ec), 4);

	return s1 + s2 + std::string(ret_str, strlen(ret_str));
}

std::string MathService::sub_wrapper(char* param_data, size_t len)
{
	sub_req req;
	sub_resp resp;
	uint32_t pkg_len;
	uint32_t ec;	// 错误标识 0未出错 -1出错
	rapidjson::Document doc_req;	// 存储DOM格式请求参数
	rapidjson::Document doc_resp;	// 存储DOM格式响应参数
	const char* ret_str = nullptr;	// 指向序列化后的响应参数
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writter(sb);	// 格式化

	if(doc_req.ParseInsitu(param_data).HasParseError())
	{
		LOG("add_wraper: parse request failed");
	}

	req.x = doc_req["x"].GetInt();
	req.y = doc_req["y"].GetInt();

	sub(req, resp);
	// 序列化返回参数
	doc_resp.Parse("{}");
	doc_resp.AddMember("err_code", resp.err_code, doc_resp.GetAllocator());
	doc_resp.AddMember("ret", resp.ret, doc_resp.GetAllocator());

	doc_resp.Accept(writter);
	ret_str = sb.GetString();

	pkg_len = 4 + strlen(ret_str);
	ec = 0;
	std::string s1((char*)&(pkg_len), 4);
	std::string s2((char*)&(ec), 4);

	return s1 + s2 + std::string(ret_str, strlen(ret_str));
}
