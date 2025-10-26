#include "LogicSystem.h"
#include <spdlog/spdlog.h>
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
        json j(msg);
        auto uid = j["uid"].get<int>();
        auto token = j["token"].get<std::string>();
        SPDLOG_INFO("User {} Login with token {}", uid, token);

        std::string return_str = j.dump(4);
        session->Send(return_str, msg_id);
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
