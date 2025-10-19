#include "ConfigManager.h"
#include "GateWayServer.h"
#include "RedisManager.h"
#include "VerifyClient.h"
#include <boost/asio.hpp>
#include <hiredis/hiredis.h>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main(int, char**)
{

    auto& cfgMgr = ConfigManager::GetInstance();
    std::cout << cfgMgr["GateWayServer"]["port"] << std::endl; // prints 9999

    try {
        unsigned short port = 9999;
        net::io_context ioc;
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code&, int) { ioc.stop(); });
        std::make_shared<GateWayServer>(ioc, port)->Start();
        ioc.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
}
