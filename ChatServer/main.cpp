#include "global/ConfigManager.h"
#include "global/const.h"
#include "grpc/ChatGrpcServer.h"
#include "redis/RedisManager.h"
#include "server/AsioPool.h"
#include "server/Server.h"

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/beast/http/field.hpp>
#include <grpc++/grpc++.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>
#include <spdlog/spdlog.h>
#include <thread>

#include <mysql++/mysql++.h>

int main()
{

    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    spdlog::set_level(spdlog::level::debug);

    auto& cfg = ConfigManager::GetInstance();
    auto server_name = cfg["SelfServer"]["name"];

    {
        RedisManager::GetInstance()->HSet(LOGIN_COUNT_PREFIX, server_name, "0");
        std::string server_address = cfg["SelfServer"]["host"] + ":" + cfg["SelfServer"]["RPCPort"];
        ChatGrpcServer service;
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        SPDLOG_INFO("Grpc Server On: {}", server_address);

        std::thread grpc_server([&server]() {
            server->Wait();
        });

        auto pool = AsioPool::GetInstance();
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, pool, &server](const boost::system::error_code& /*error*/, int /*signal_number*/) {
            pool->Stop();
            ioc.stop();
            server->Shutdown();
        });

        auto port = cfg["SelfServer"]["port"];
        std::make_shared<Server>(ioc, std::stoi(port))->Start();
        ioc.run();

        RedisManager::GetInstance()->HDel(LOGIN_COUNT_PREFIX, server_name);
        RedisManager::GetInstance()->Close();
        grpc_server.join();
    }
    return 0;
}
