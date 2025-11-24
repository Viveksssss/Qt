// Standard Library
#include <memory>
#include <thread>

// Third-party Libraries
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <grpc++/grpc++.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>
#include <spdlog/spdlog.h>

// Project Headers
#include "global/ConfigManager.h"
#include "global/const.h"
#include "grpc/ChatGrpcServer.h"
#include "redis/RedisManager.h"
#include "server/AsioPool.h"
#include "server/Server.h"

int main()
{
    // 1. 初始化日志记录器
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    spdlog::set_level(spdlog::level::debug);

    // 使用一个作用域来管理资源生命周期
    {
        // 2. 加载配置并初始化 Redis
        auto& cfg = ConfigManager::GetInstance();
        auto server_name = cfg["SelfServer"]["name"];
        RedisManager::GetInstance()->HSet(LOGIN_COUNT_PREFIX, server_name, "0");

        // 3. 设置并启动 gRPC 服务器
        std::string grpc_server_address = cfg["SelfServer"]["host"] + ":" + cfg["SelfServer"]["RPCPort"];
        ChatGrpcServer grpc_service;
        grpc::ServerBuilder builder;
        builder.AddListeningPort(grpc_server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&grpc_service);

        std::unique_ptr<grpc::Server> grpc_server(builder.BuildAndStart());
        SPDLOG_INFO("gRPC Server listening on {}", grpc_server_address);

        std::thread grpc_thread([&grpc_server]() {
            grpc_server->Wait();
        });

        // 4. 设置并启动 Asio TCP 服务器
        auto asio_pool = AsioPool::GetInstance();
        boost::asio::io_context ioc;
        auto port = cfg["SelfServer"]["port"];
        std::make_shared<Server>(ioc, std::stoi(port))->Start();

        // 5. 设置信号处理以实现优雅停机
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, asio_pool, &grpc_server](const boost::system::error_code& /*error*/, int /*signal_number*/) {
            SPDLOG_INFO("Shutdown signal received.");
            // 首先停止 gRPC 服务器以拒绝新请求
            grpc_server->Shutdown();
            // 停止 Asio 线程池
            asio_pool->Stop();
            // 停止 Asio 事件循环
            ioc.stop();
        });

        // 6. 运行 Asio 事件循环 (此调用会阻塞，直到 ioc.stop() 被调用)
        SPDLOG_INFO("Chat Server listening on port {}", port);
        ioc.run();

        // 7. 清理资源
        SPDLOG_INFO("Cleaning up resources...");
        RedisManager::GetInstance()->HDel(LOGIN_COUNT_PREFIX, server_name);
        RedisManager::GetInstance()->Close();
        grpc_thread.join();
        SPDLOG_INFO("Server shutdown complete.");
    }

    return 0;
}
