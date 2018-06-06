#include "trigger.h"
#include <iostream>
#include <boost/format.hpp>
#include <boost/asio.hpp>

class StdinTrigger : public Trigger
{
public:
    StdinTrigger()
    {
    }
    void wait()
    {
        std::cout << "Input any key and Enter..." << std::endl;
        std::string tmp;
        std::cin >> tmp;
    }
};

class TcpTrigger : public Trigger
{
public:
    TcpTrigger()
    {
    }
    void wait()
    {
        std::cout << "Waiting button push..." << std::endl;
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::socket sock(io_service);
        u_short port = 4123;
        boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
        acceptor.accept(sock);
        boost::asio::streambuf receive_buffer;
        boost::system::error_code error;
        boost::asio::read(sock, receive_buffer, boost::asio::transfer_at_least(1), error);
    }
};

std::shared_ptr<Trigger> Trigger::create()
{
#ifdef ENABLE_RASPBERRY_PI_CAMERA
    return std::make_shared<TcpTrigger>();
#else
    return std::make_shared<TcpTrigger>();
#endif
}
