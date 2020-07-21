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
#include "asio.hpp"
#include "src/T_MathParam.h"

using namespace std;
using asio::ip::tcp;

enum { max_length = 1024 };

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
    uint32_t len = 8 + reqstr.size() + sizeof(req);
    asio::write(s, asio::buffer(&len, 4));
    asio::write(s, asio::buffer(&sid, 8));
    asio::write(s, asio::buffer(reqstr.c_str(), reqstr.size()));
    asio::write(s, asio::buffer(&req, sizeof(req)));
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
    memcpy((void*)&resp, (void*)reply, len-4);
    std::cout << "Reply is: ";
    cout << resp.ret << endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
