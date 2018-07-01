#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <thread>
#include <iostream>

/**
 boost::asioのconnectをタイムアウトするために作成したクラス
 */
class MyClient {
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::deadline_timer connect_timer_;
    bool connect_result_;
    bool callbacked_;

    void on_connect(boost::system::error_code const & error)
    {
        callbacked_ = true;
        if (error) {
            std::cout << "[FATAL] on_connect: " << error.message() << std::endl;
            return;
        }
        connect_timer_.cancel();
        connect_result_ = true;
    }
    
    void on_connect_timeout(boost::system::error_code const & error)
    {
        callbacked_ = true;
        std::cout << "[CAUTION] connection timeout !" << std::endl;
        if (!error) {
            socket_.close();
        }
    }
public:
    MyClient(boost::asio::io_service & io_service)
        : io_service_(io_service)
        , socket_(io_service)
        , connect_timer_(io_service)
        , connect_result_(false)
        , callbacked_(false)
    {}
    
    bool connect(std::string url, int port, int timeout_sec)
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(url), port);
        std::cout << "connecting..." << std::endl;
        socket_.async_connect(ep, boost::bind(&MyClient::on_connect, this, _1));
        connect_timer_.expires_from_now(boost::posix_time::seconds(timeout_sec));
        connect_timer_.async_wait(boost::bind(&MyClient::on_connect_timeout, this, _1));
        while(callbacked_){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        std::cout << connect_result_ << std::endl;
        return connect_result_;
    }
    
    boost::asio::ip::tcp::socket & ref_socket() {
        return socket_;
    }
};
