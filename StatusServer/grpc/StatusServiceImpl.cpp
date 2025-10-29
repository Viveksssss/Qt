#include "StatusServiceImpl.h"
#include "../global/ConfigManager.h"
#include "../global/const.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <mutex>
#include <spdlog/spdlog.h>

std::string generate_unique_string()
{
    boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();
    return boost::uuids::to_string(uuid);
}

grpc::Status StatusServiceImpl::GetChatServer(grpc::ServerContext* context, const message::GetChatServerRequest* request, message::GetChatServerResponse* response)
{
    std::string prefix("ChatServer received :");
    const auto& server = GetChatServer();
    response->set_host(server.host);
    response->set_port(server.port);
    response->set_error(static_cast<int>(ErrorCodes::SUCCESS));
    response->set_token(generate_unique_string());
    insertToken(request->uid(), response->token());
    SPDLOG_INFO("{} uid:{}, token:{}, host:{}, port:{}", prefix, request->uid(), response->token(), server.host, server.port);
    return grpc::Status::OK;
}

void StatusServiceImpl::insertToken(int uid, const std::string& token)
{
    std::lock_guard<std::mutex> lock(_token_mutex);
    _tokens[uid] = token;
}

ChatServer StatusServiceImpl::GetChatServer()
{
    std::lock_guard<std::mutex> lock(_server_mutex);
    return _servers.top();
}

grpc::Status StatusServiceImpl::Login(grpc::ServerContext* context, const message::LoginRequest* request, message::LoginResponse* response)
{
    auto uid = request->uid();
    auto token = request->token();

    std::lock_guard<std::mutex> lock(_token_mutex);
    auto it = _tokens.find(uid);
    if (it == _tokens.end()) {
        response->set_error(static_cast<int>(ErrorCodes::ERROR_UID_INVALID));
    } else if (it->second != token) {
        response->set_error(static_cast<int>(ErrorCodes::ERROR_TOKEN_INVALID));
    }
    response->set_error(static_cast<int>(ErrorCodes::SUCCESS));
    response->set_uid(uid);
    response->set_token(token);
    return grpc::Status::OK;
}

StatusServiceImpl::StatusServiceImpl()
{
    auto& cfg = ConfigManager::GetInstance();
    ChatServer server;
    server.port = cfg["ChatServer"]["port"];
    server.host = cfg["ChatServer"]["host"];
    _servers.push(server);
}
