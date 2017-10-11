#include "sender.h"
#include <boost/asio.hpp>

namespace
{
	std::string makeJson(Option const & opt, std::vector<BlockInfo> const & blockInfo)
	{
		std::stringstream ss;
		ss << "show:{\"orders\":[";
		for (size_t i = 0; i < blockInfo.size(); ++i){
			ss << "{\"id\":\"" << opt.clr2inst.at(blockInfo[i].color.name) << "\",\"lifetime\":3,\"param\":{}}";
			if (i < blockInfo.size() - 1){
				ss << ",";
			}
		}
		ss << "]}";
		return ss.str();
	}

	void sendTcp(std::string const & data, std::string const & address, int port)
	{
		namespace asio = boost::asio;
		namespace ip = asio::ip;
		/*
		{
		std::cout << "connecting... " << address << ":" << port << std::endl;
		ip::tcp::iostream stream(address, boost::lexical_cast<std::string>(port));
		stream << json << std::endl;
		std::string res;
		std::getline(stream, res);
		}
		*/
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
