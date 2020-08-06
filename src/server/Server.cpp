
#include "asio.hpp"

#include <memory>
#include <utility>
#include <cstdlib>

class Session 
    : public std::enable_shared_from_this<Session>
{
public:
    Session(asio::ip::tcp::socket socket): socket_(std::move(socket)) {}

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, 1024),
            [this, self](std::error_code ec, std::size_t length)
            {
                if(!ec)
                {
                    do_write(length);
                }
            });
    }

    void do_write(size_t length)
    {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
            [this, self](std::error_code ec, std::size_t /* length */)
            {
                if(!ec)
                {
                    do_read();
                }
            });
    }

private:
    asio::ip::tcp::socket socket_;
    char data_[1024];
};

class Server
{
public:
    Server();

    void run();

private:
    void do_accept();

private:
    asio::io_context ctx_;
    short port;
    asio::ip::tcp::acceptor acceptor_;
};

Server::Server(): ctx_(), port(3000), acceptor_(ctx_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

void Server::run() 
{
    do_accept();
    ctx_.run();
    printf("end\n");
}

void Server::do_accept() 
{
    acceptor_.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket)
        {
            if(!ec) 
            {
                std::make_shared<Session>(std::move(socket))->start();
            }
            do_accept();
        }
    );
}

int main()
{
    Server s;
    s.run();
}
