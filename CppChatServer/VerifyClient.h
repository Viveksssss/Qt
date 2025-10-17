#ifndef VERIFYCLIENT_H
#define VERIFYCLIENT_H

#include "ConfigManager.h"
#include "Singleton.h"
#include "const.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <atomic>
#include <condition_variable>
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <mutex>
#include <queue>

using message::GetSecurityCodeRequest;
using message::GetSecurityCodeResponse;
using message::VarifyService;

class RPCPool {
public:
    RPCPool(std::size_t size, const std::string& host, const std::string& port)
        : _size(size)
        , _host(host)
        , _port(port)
        , _stop(false)
    {
        _channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
        for (std::size_t i = 0; i < size; ++i) {
            _connections.push(VarifyService::NewStub(_channel));
        }
    }

    void Close()
    {
        _stop = true;
        _cv.notify_all();
    }

    ~RPCPool()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        Close();
        while (!_connections.empty()) {
            _connections.pop();
        }
    }

    std::unique_ptr<VarifyService::Stub> GetConnection()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this]() {
            return _stop || !_connections.empty();
        });
        if (_stop) {
            return nullptr;
        }

        auto context = std::move(_connections.front());
        _connections.pop();
        return context;
    }

    void backConnection(std::unique_ptr<VarifyService::Stub> context)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_stop) {
            return;
        }
        _connections.push(std::move(context));
        _cv.notify_one();
    }

private:
    std::shared_ptr<grpc::Channel> _channel;
    std::atomic<bool> _stop;
    std::size_t _size;
    std::string _host;
    std::string _port;
    std::queue<std::unique_ptr<VarifyService::Stub>> _connections;
    std::mutex _mutex;
    std::condition_variable _cv;
};

class VerifyClient : public Singleton<VerifyClient> {
    friend class Singleton<VerifyClient>;

public:
    GetSecurityCodeResponse GetSecurityCode(const std::string& email)
    {
        grpc::ClientContext context;
        GetSecurityCodeRequest request;
        GetSecurityCodeResponse response;
        request.set_email(email);

        auto stub = _pool->GetConnection();
        grpc::Status status = stub->GetSecurityCode(&context, request, &response);

        if (!status.ok()) {
            response.set_error(static_cast<int>(ErrorCodes::RPCFAILED));
        }
        _pool->backConnection(std::move(stub));

        return response;
    }

private:
    VerifyClient()
    {
        auto& cfgMgr = ConfigManager::GetInstance();
        std::string host = cfgMgr["VarifyServer"]["host"];
        std::string port = cfgMgr["VarifyServer"]["port"];
        _pool = std::make_unique<RPCPool>(10, host, port);
    }

    std::unique_ptr<RPCPool> _pool;
};

#endif