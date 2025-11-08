#include "ChatGrpcServer.h"

ChatGrpcServer::ChatGrpcServer()
{
}
// TODO:
Status ChatGrpcServer::NotifyAddFriend(grpc::ServerContext* context, const AddFriendRequest* request, AddFriendResponse* response)
{
    return Status::OK;
}
Status ChatGrpcServer::NotifyAuthFriend(grpc::ServerContext* context, const AuthFriendRequest* request, AuthFriendResponse* response)
{
    return Status::OK;
}
Status ChatGrpcServer::NotifyTextChatMessage(grpc::ServerContext* context, const TextChatMessageRequest* request, TextChatMessageResponse* response)
{
    return Status::OK;
}
bool ChatGrpcServer::ChatGrpcServer::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
    return true;
}
