//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include "asio.hpp"
#include "src/T_MathParam.h"

using namespace std;
using asio::ip::tcp;
using namespace rapidjson;

enum { max_length = 10240 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(s, resolver.resolve(argv[1], argv[2]));

    uint64_t sid = hash<string>()("Math");
    string reqstr("add\n");
    add_req req;
    req.x = 2;
    req.y = 5;
    Document doc_req;
    StringBuffer sb;
    PrettyWriter<StringBuffer> writter(sb);
    doc_req.Parse("{}");
    doc_req.AddMember("x", req.x, doc_req.GetAllocator());
    doc_req.AddMember("y", req.y, doc_req.GetAllocator());
    //printf("add end\n");
    doc_req.Accept(writter);
    //printf("here\n");
    auto req_str = sb.GetString();
    printf("%s\n", req_str);
    uint32_t len = 8 + reqstr.size() + strlen(req_str);
    printf("send pkg len: %u, req_str len is %u\n", len, strlen(req_str));
    asio::write(s, asio::buffer(&len, 4));
    asio::write(s, asio::buffer(&sid, 8));
    asio::write(s, asio::buffer(reqstr.c_str(), reqstr.size()));
    asio::write(s, asio::buffer(req_str, strlen(req_str)));
    cout << "send msg finish\n";
    char reply[max_length];
    asio::read(s, asio::buffer(&len, 4));
    cout << "read pkg length: " << len << endl;
    uint32_t ec;
    asio::read(s, asio::buffer(&ec, 4));
    cout << "error code: " << ec << endl;
    if(ec != 0) {
      //cout << ec << endl;
      return 0;
    }
    asio::read(s, asio::buffer(reply, len-4));
    add_resp resp;
    Document doc;
    if(doc.ParseInsitu(reply).GetParseError()) {
      cout << "parse resp error\n";
      return 1;
    }
    //memcpy((void*)&resp, (void*)reply, len-4);
    resp.err_code = doc["err_code"].GetInt();
    resp.ret = doc["ret"].GetInt();
    std::cout << "Reply is: ";
    cout << resp.ret << endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
