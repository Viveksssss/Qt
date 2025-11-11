#ifndef CHATGRPCSERVER_H
#define CHATGRPCSERVER_H

#include "../data/UserInfo.h"
#include "RPCPool.h"
#include "message.grpc.pb.h"
#include "message.pb.h"

#include <grpcpp/server_context.h>
#include <nlohmann/json.hpp>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>


using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::ChatServer;

using message::AddFriendRequest;
using message::AddFriendResponse;

using message::AuthFriendRequest;
using message::AuthFriendResponse;

using message::GetChatServerResponse;
using message::GetChatServerRequest;

using message::TextChatMessageRequest;
using message::TextChatMessageResponse;
using message::TextChatData;
class ChatGrpcServer final:public message::ChatServer::Service
{
public:
    ChatGrpcServer();
    Status NotifyAddFriend(grpc::ServerContext*context,const AddFriendRequest*request,AddFriendResponse*response)override;
    Status NotifyAuthFriend(grpc::ServerContext*context,const AuthFriendRequest*request,AuthFriendResponse*response)override;
    Status NotifyTextChatMessage(grpc::ServerContext*context,const TextChatMessageRequest*request,TextChatMessageResponse*response)override;
    bool GetBaseInfo(std::string base_key,int uid,std::shared_ptr<UserInfo>&userinfo);
private:
};

#endif
