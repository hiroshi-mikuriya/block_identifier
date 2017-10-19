#include "sender.h"
#include "picojson.h"
#include <boost/asio.hpp>
#include <boost/format.hpp>

namespace
{
    double getValue(Instruction::Param const & param, int type)
    {
        if (Instruction::Param::Int == param.type){
            return type;
        }
        if (Instruction::Param::Double == param.type){
            return type;
        }
        throw std::runtime_error((boost::format("unsupported instruction param type. [%s]") % param.type).str());
    }

    std::string makeJson(Option const & opt, std::vector<BlockInfo> const & blockInfo)
    {
        using value = picojson::value;
        picojson::array orders;
        for (auto info : blockInfo){
            picojson::object item;
            auto const instname = opt.clr2inst.at(info.color.name);
            item["id"] = value(instname);
            item["lifetime"] = value(3.0);
            auto inst = std::find_if(opt.insts.begin(), opt.insts.end(), [instname](Instruction const & v){
                return instname == v.name;
            });
            if (inst != opt.insts.end()){
                for (auto param : inst->params){
                    item[param.name] = value(getValue(param, info.type));
                }
            }
            orders.emplace_back(item);
        }
        picojson::object root;
        root["orders"] = value(orders);
        return value(root).serialize();
    }

    /*void sendTcp(std::string const & data, std::string const & address, int port)
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
    }*/

    void postJson(
        std::string host,
        int port,
        std::string path,
        std::string json)
    {
#define NEW_LINE    "\r\n"
        boost::asio::io_service io_service;
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
                << "POST " << path << " HTTP/1.1" NEW_LINE
                << "Host: " << host << NEW_LINE
                << "Accept: */*" NEW_LINE
                << "Content-Length: " << json.size() << NEW_LINE
                << "Content-Type: application/json" NEW_LINE
                << "Connection: Close" NEW_LINE
                << NEW_LINE
                << json
                ;
            std::cout << "MESSAGE :\n" << json << "\n" << std::endl;
            boost::asio::write(sock, request);
        }

        // responce
        {
            boost::asio::streambuf response;
            boost::asio::read_until(sock, response, NEW_LINE);
            std::istream res_s(&response);
            std::string http_version;
            unsigned int status_code;
            res_s >> http_version;
            res_s >> status_code;
            std::vector<char> buf(1000);
            res_s.read(buf.data(), buf.size());
            buf.push_back(0);
            const char * target = NEW_LINE NEW_LINE;
            auto p = std::strstr(buf.data(), target);
            if (!p){
                std::cout << "Not found respoce message." << std::endl;
                return;
            }
            std::cout
                << boost::format("%-12s : %s\n") % "HTTP VERSION" % http_version
                << boost::format("%-12s : %d\n") % "STATUS CODE" % status_code
                << boost::format("%-12s : %s\n") % "MESSAGE" % (p + strlen(target))
                ;
        }
#undef NEW_LINE
    }
}

void sendToServer(Option const & opt, std::vector<BlockInfo> const & blockInfo, std::string const & address, int port)
{
    try{
        if (blockInfo.empty()){
            throw std::runtime_error("block count should be natural number.");
        }
        auto const data = makeJson(opt, blockInfo);
        if (address.empty()){
            std::cout << data << std::endl;
        }
        else{
            postJson(address, port, "/api/show", data);
        }
    }
    catch (std::exception const & e) {
        std::cerr << e.what() << std::endl;
    }
}
