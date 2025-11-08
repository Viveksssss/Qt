#include "LogicSystem.h"
#include "../data/UserInfo.h"
#include "../global/UserManager.h"
#include "../global/const.h"
#include "../grpc/StatusClient.h"
#include "../mysql/MysqlManager.h"
#include "../redis/RedisManager.h"
#include <boost/mpl/base.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>

std::string thread_id_to_string(std::thread::id id)
{
    std::stringstream ss;
    ss << id;
    return ss.str();
}

void LogicSystem::PostMsgToQueue(std::shared_ptr<LogicNode> msg)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _queue.push(msg);
    _cv.notify_one();
}

void LogicSystem::RegisterCallBacks()
{
    // 登陆请求
    _function_callbacks[MsgId::ID_CHAT_LOGIN] = [this](std::shared_ptr<Session> session, uint16_t msg_id, const std::string& msg) {
        json j = json::parse(msg);
        auto uid = j["uid"].get<int>();
        auto token = j["token"].get<std::string>();
        SPDLOG_INFO("Thread: {},User {} Login with token {}", thread_id_to_string(std::this_thread::get_id()), uid, token);

        json jj;
        Defer defer([this, &jj, session]() {
            std::string return_str = jj.dump();
            session->Send(return_str, static_cast<int>(MsgId::ID_CHAT_LOGIN_RSP));
        });

        std::string uid_str = std::to_string(uid);
        std::string token_key = USER_TOKEN_PREFIX + uid_str;
        std::string token_value = "";

        bool success = RedisManager::GetInstance()->Get(token_key, token_value);
        if (!success) {
            jj["error"] = ErrorCodes::ERROR_UID_INVALID;
            return;
        }
        if (token_value != token) {
            jj["error"] = ErrorCodes::ERROR_TOKEN_INVALID;
            return;
        }

        std::string base_key = USER_BASE_INFO_PREFIX + uid_str;
        auto user_info = std::make_shared<UserInfo>();
        bool b_base = GetBaseInfo(base_key, uid, user_info);
        if (!b_base) {
            jj["error"] = ErrorCodes::ERROR_UID_INVALID;
            return;
        }

        jj["error"] = ErrorCodes::SUCCESS;

        jj["uid"] = uid;
        jj["name"] = user_info->name;
        jj["email"] = user_info->email;
        jj["nick"] = user_info->nick;
        jj["sex"] = user_info->sex;
        jj["desc"] = user_info->desc;
        jj["icon"] = user_info->icon;
        jj["token"] = token;

        // 获取消息列表
        // 获取好友列表

        // 更新登陆数量
        auto server_name = ConfigManager::GetInstance()["SelfServer"]["name"];
        auto count_str = RedisManager::GetInstance()->HGet(LOGIN_COUNT_PREFIX, server_name);
        int count = 0;
        if (!count_str.empty()) {
            count = std::stoi(count_str);
        }
        count++;
        count_str = std::to_string(count);
        RedisManager::GetInstance()->HSet(LOGIN_COUNT_PREFIX, server_name, count_str);

        // session绑定uid
        session->SetUid(uid);
        // 绑定连接的服务器名称和用户uid
        std::string ip_key = USERIP_PREFIX + std::to_string(uid);
        RedisManager::GetInstance()->Set(ip_key, server_name);
        // uid和session绑定管理，方便之后踢人
        UserManager::GetInstance()->SetUserSession(uid, session);
    };
}

void LogicSystem::DealMsg()
{
    while (true) {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this]() {
            return _stop || !_queue.empty();
        });
        if (_stop && _queue.empty()) {
            break;
        }

        if (!_queue.empty()) {
            std::queue<std::shared_ptr<LogicNode>> local_queue;
            local_queue.swap(_queue);
            lock.unlock();
            while (!local_queue.empty()) {
                auto msg = local_queue.front();
                local_queue.pop();

                auto it = _function_callbacks.find(static_cast<MsgId>(msg->_recv_node->_msg_id));
                if (it != _function_callbacks.end()) {
                    it->second(msg->_session, msg->_recv_node->_msg_id, std::string(msg->_recv_node->_data, msg->_recv_node->_total_len));
                }
            }
        }
    }
}

bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
    std::string info_str = "";
    bool b_base = RedisManager::GetInstance()->Get(base_key, info_str);
    if (b_base) {
        json j = json::parse(info_str);
        userinfo->name = j["name"].get<std::string>();
        userinfo->email = j["email"].get<std::string>();
        userinfo->uid = j["uid"].get<int>();
        userinfo->sex = j["sex"].get<int>();
        userinfo->nick = j["nick"].get<std::string>();
        userinfo->desc = j["desc"].get<std::string>();
        userinfo->icon = j["icon"].get<std::string>();
        SPDLOG_INFO("uid:{},name:{},email:{}", uid, userinfo->name, userinfo->email);
    } else {
        userinfo = MysqlManager::GetInstance()->GetUser(uid);
        if (userinfo == nullptr) {
            return false;
        }
        json j;
        j["name"] = userinfo->name;
        j["email"] = userinfo->email;
        j["uid"] = userinfo->uid;
        j["sex"] = userinfo->sex;
        j["nick"] = userinfo->nick;
        j["desc"] = userinfo->desc;
        j["icon"] = userinfo->icon;
        RedisManager::GetInstance()->Set(base_key, j.dump());
    }
    return true;
}

LogicSystem::LogicSystem(std::size_t size)
    : _stop(false)
    , _size(size)
{
    RegisterCallBacks();
    _work_threads.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        _work_threads.emplace_back(&LogicSystem::DealMsg, this);
    }
}

LogicSystem::~LogicSystem()
{
    _stop = true;
    _cv.notify_all();
    for (auto& p : _work_threads) {
        p.join();
    }

    SPDLOG_INFO("LogicSystem Stopped");
}
