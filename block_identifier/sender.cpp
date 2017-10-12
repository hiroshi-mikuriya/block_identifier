#include "sender.h"
#include "picojson.h"
#include <boost/asio.hpp>

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
        picojson::object show;
        show["orders"] = value(orders);
        picojson::object root;
        root["show"] = value(show);
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
}

void sendTcp(Option const & opt, std::vector<BlockInfo> const & blockInfo, std::string const & address, int port)
{
    try{
        auto const json = makeJson(opt, blockInfo);
        std::cout << json << std::endl;
        sendTcp(json, address, port);
    }
    catch (std::exception const & e) {
        std::cerr << e.what() << std::endl;
    }
}
