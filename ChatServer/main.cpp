#include "global/ConfigManager.h"
#include "server/AsioPool.h"
#include "server/Server.h"

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <grpc++/grpc++.h>
#include <spdlog/spdlog.h>
#include <thread>

int main()
{
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    spdlog::set_level(spdlog::level::debug);

    {
        auto& cfg = ConfigManager::GetInstance();
        auto pool = AsioPool::GetInstance();
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, pool](const boost::system::error_code& /*error*/, int /*signal_number*/) {
            pool->Stop();
            ioc.stop();
        });

        auto port = cfg["ChatServer"]["port"];
        std::make_shared<Server>(ioc, std::stoi(port))->Start();
        ioc.run();
    }
    return 0;
}