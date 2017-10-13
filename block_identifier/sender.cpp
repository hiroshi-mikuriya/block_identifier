#include "sender.h"
#include "picojson.h"
#include <boost/asio.hpp>
#include <boost/format.hpp>

namespace
{
    std::string makeJson(Option const & opt, std::vector<BlockInfo> const & blockInfo)
    {
        using value = picojson::value;
        picojson::array orders;
        for (auto info : blockInfo){
            picojson::object item;
            item["id"] = value(opt.clr2inst.at(info.color.name));
            item["lifetime"] = value(3.0);
            item["param"] = value(picojson::object());
            orders.emplace_back(item);
        }
        picojson::object root;
        root["orders"] = value(orders);
        return value(root).serialize();
    }

    void sendTcp(std::string const & data, std::string const & address, int port)
    {
        namespace asio = boost::asio;
        namespace ip = asio::ip;
        asio::io_service io_service;
        asio::ip::tcp::socket sock(io_service);
        std::cout << "connecting... " << address << ":" << port << std::endl;
        sock.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(address), port));
        std::cout << "sending..." << std::endl;
        write(sock, asio::buffer(data));
        std::cout << "finished" << std::endl;
    }

    void httpPost(
        boost::asio::io_service& io_service,
        std::string host,
        int port,
        std::string path,
        std::string message,
        std::string room)
    {
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(host, "http");
        boost::asio::ip::tcp::socket sock(io_service);
        std::cout << "connecting... " << host << std::endl;
        sock.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
        std::cout << "connected." << std::endl;

        // request
        {
            boost::asio::streambuf request;
            std::ostream req_s(&request);
            req_s
                << "POST " << path << room << " HTTP/1.1\r\n"
                << "Host: " << host << "\r\n"
                << "Accept: */*\r\n"
                << "Content-Length: " << message.size() << "\r\n"
                << "Content-Type: application/x-www-form-urlencoded\r\n"
                << "Connection: Close\r\n"
                << "\r\n"
                << message
                ;
            std::cout << "MESSAGE :\n" << message << "\n" << std::endl;
            boost::asio::write(sock, request);
        }

        // responce
        {
            boost::asio::streambuf response;
            boost::asio::read_until(sock, response, "\r\n");
            std::istream res_s(&response);
            std::string http_version, status_text, message;
            unsigned int status_code;
            res_s >> http_version;
            res_s >> status_code;
            res_s >> status_text;
            res_s >> message;
            std::cout
                << boost::format("%-12s : %s\n") % "HTTP VERSION" % http_version
                << boost::format("%-12s : %d\n") % "STATUS CODE" % status_code
                << boost::format("%-12s : %d\n") % "STATUS TEXT" % status_text
                << boost::format("%-12s : %s\n") % "MESSAGE" % message
                ;
        }
    }

    void sendHttp(std::string const & data, std::string const & address, int port)
    {
        boost::asio::io_service io_service;
        httpPost(io_service, address, port, "/show", data, "");
    }
}

void sendToServer(Option const & opt, std::vector<BlockInfo> const & blockInfo, std::string const & address, int port)
{
    try{
        if (blockInfo.empty()){
            throw std::runtime_error("block count should be natural number.");
        }
        auto const data = makeJson(opt, blockInfo);
        sendHttp(data, address, port);
    }
    catch (std::exception const & e) {
        std::cerr << e.what() << std::endl;
    }
}
