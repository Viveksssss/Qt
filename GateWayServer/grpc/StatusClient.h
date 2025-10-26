#ifndef STATUCCLIENT_H
#define STATUCCLIENT_H

#include "../global/ConfigManager.h"
#include "../global/Singleton.h"
#include "../global/const.h"
#include "RPCPool.h"

using message::GetChatServerRequest;
using message::GetChatServerResponse;
using message::StatusService;

class StatusClient : public Singleton<StatusClient> {
    friend class Singleton<StatusClient>;

public:
    ~StatusClient() = default;
    GetChatServerResponse GetChatServer(int uid);

private:
    StatusClient();

    std::unique_ptr<RPCPool<StatusService, StatusService::Stub>> _pool;
};

#endif