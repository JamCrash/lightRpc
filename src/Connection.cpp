
#include "Connection.h"
#include "ServiceManager.h"

#include "Log.h"

Connection::Connection(tcp::socket socket, ServiceManagerPtr svcManager)
: socket_(std::move(socket)),
  serviceManager_(svcManager)
{}

void Connection::start()
{
    do_read_pkg_length();
}

void Connection::stop()
{
    socket_.close();
}

void Connection::do_read_pkg_length()
{
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(&pkgLen_, sizeof(pkgLen_)), 
        [this, self](std::error_code ec, size_t /*length*/)
        {
            if(!ec)
            {
                // TODO: 判断pkgLen_大小是否超过buffer_大小 2020年07月16日00:44:26
                LOG_TEST(pkgLen_);
                do_read_pkg();
            }
            // TODO: else(不同ec的处理方式) 2020年07月16日00:04:04
            // else
        });
}

void Connection::do_read_pkg()
{
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(buffer_, pkgLen_), 
        [this, self](std::error_code ec, size_t /*length*/)
        { 
            if(!ec)
            {
                do_write(serviceManager_->dispatch(buffer_, pkgLen_));
            }
            // TODO: else(不同ec的处理方式) 2020年07月16日00:09:08
            else
            {
                LOG("do_read_pkg: early terminate");
            }
        });
}

void Connection::do_write(std::string response)
{
    auto self(shared_from_this());
    LOG_TEST(response.c_str());

    asio::async_write(socket_, asio::buffer(response.c_str(), response.size()),
        [this, self](std::error_code ec, size_t /*length*/)
        {
            if(ec)
            {
                // TODO: 错误处理 2020年07月18日12:25:51
                LOG("do_write error");
            }
            do_read_pkg_length();
        });
}
