#include "ChatGrpcClient.h"
#include "../global/ConfigManager.h"
#include "message.pb.h"
#include <grpcpp/client_context.h>
#include <new>
#include <spdlog/spdlog.h>
#include <string>

AddFriendResponse ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendRequest& req)
{

    AddFriendResponse rsp;
    Defer defer([&rsp, &req]() {
        rsp.set_error(static_cast<int>(ErrorCodes::SUCCESS));
        rsp.set_fromuid(req.fromuid());
        rsp.set_touid(req.touid());
    });

    auto it = _pool.find(server_ip);
    if (it == _pool.end()) {
        return rsp;
    }

    auto& pool = it->second;
    grpc::ClientContext context;
    auto stub = pool->GetConnection();
    Defer defer2([&pool, &stub]() {
        pool->ReturnConnection(std::move(stub));
    });

    Status status = stub->NotifyAddFriend(&context, req, &rsp);
    if (!status.ok()) {
        rsp.set_error(static_cast<int>(ErrorCodes::RPCFAILED));
        return rsp;
    }

    return rsp;
}

AuthFriendResponse ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendRequest&)
{
    AuthFriendResponse rsp;

    return rsp;
}

TextChatMessageResponse ChatGrpcClient::NotifyTextChatMessage(std::string server_ip, const TextChatMessageRequest& req, const json&)
{
    TextChatMessageResponse rsp;
    return rsp;
}
bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
    return true;
}

ChatGrpcClient::ChatGrpcClient()
{
    auto& cfg = ConfigManager::GetInstance();
    auto server_list = cfg["PeerServer"]["servers"];

    std::vector<std::string> words;
    words.reserve(10);

    std::stringstream ss(server_list);
    std::string word;

    while (std::getline(ss, word, ',')) {
        words.push_back(word);
    }

    for (const auto& word : words) {
        if (cfg[word]["name"].empty()) {
            continue;
        }
        _pool[cfg[word]["name"]] = std::make_unique<RPCPool<ChatServer, ChatServer::Stub>>(10, cfg[word]["host"], cfg[word]["RPCPort"]);
    }
}
