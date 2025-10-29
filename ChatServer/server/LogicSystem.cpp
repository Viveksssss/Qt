#include "LogicSystem.h"
#include "../data/UserInfo.h"
#include "../grpc/StatusClient.h"
#include "../mysql/MysqlManager.h"
#include "../redis/RedisManager.h"
#include <boost/mpl/base.hpp>
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

        auto reply = StatusClient::GetInstance()->Login(uid, token);
        json jj;
        Defer defer([this, &jj, session]() {
            std::string return_str = jj.dump(4);
            session->Send(return_str, static_cast<int>(MsgId::ID_CHAT_LOGIN_RSP));
        });

        jj["error"] = reply.error();
        if (reply.error() != static_cast<int>(ErrorCodes::SUCCESS)) {
            return;
        }

        std::string base_key = USER_BASE_INFO + std::to_string(uid);
        auto user_info = std::make_shared<UserInfo>();
        bool b_base = GetBaseInfo(base_key, uid, user_info);
        if (!b_base) {
            jj["error"] = ErrorCodes::ERROR_UID_INVALID;
            return;
        }
        jj["uid"] = uid;
        jj["name"] = user_info->name;
        jj["email"] = user_info->email;
        jj["password"] = user_info->password;
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
        // userinfo->uid = j["uid"].get<int>();
        // userinfo->sex = j["sex"].get<int>();
        userinfo->name = j["name"].get<std::string>();
        userinfo->password = j["password"].get<std::string>();
        userinfo->email = j["email"].get<std::string>();
        // userinfo->nick = j["nick"].get<std::string>();
        // userinfo->desc = j["desc"].get<std::string>();
        // userinfo->icon = j["icon"].get<std::string>();
        SPDLOG_INFO("uid:{},name:{},email:{}", uid, userinfo->name, userinfo->email);
    } else {
        userinfo = MysqlManager::GetInstance()->GetUser(uid);
        if (userinfo == nullptr) {
            return false;
        }

        json j;
        j["name"] = userinfo->name;
        j["email"] = userinfo->email;
        j["password"] = userinfo->password;

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
