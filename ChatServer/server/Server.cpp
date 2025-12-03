#include "Server.h"
#include "../global/ConfigManager.h"
#include "../global/UserManager.h"
#include "../session/Session.h"
#include "AsioPool.h"
#include "LogicSystem.h"
#include <boost/system/detail/error_code.hpp>
#include <chrono>
#include <mutex>
#include <spdlog/spdlog.h>

Server::Server(net::io_context &ioc, uint16_t port)
    : _ioc(ioc)
    , _acceptor(ioc, net::ip::tcp::endpoint(net::ip::tcp::v4(), port))
    , _port(port)
    , _timer(_ioc, std::chrono::seconds(30)) {
    SPDLOG_INFO("Server Start Success,Listen on port:{}", _port);
    auto &cfg = ConfigManager::GetInstance();
    _server_name = cfg["SelfServer"]["name"];
}

Server::~Server() {
    StopTimer();
    _sessions.clear();
}

void Server::Start() {
    auto &io_context = AsioPool::GetInstance()->GetIOService();
    std::shared_ptr<Session> conn =
        std::make_shared<Session>(io_context, shared_from_this());
    _acceptor.async_accept(
        conn->GetSocket(), [this, conn, self = shared_from_this()](
                               const boost::system::error_code &ec) {
            try {
                if (ec) {
                    self->Start();
                    return;
                }
                if (!_timer_running) {
                    self->StartTimer();
                }

                conn->Start();
                SPDLOG_INFO(
                    "New connection from {},session:{}",
                    conn->GetSocket().remote_endpoint().address().to_string(),
                    conn->GetSessionId());

                std::unique_lock<std::mutex> lock(_mutex);
                _sessions.insert(std::make_pair(conn->GetSessionId(), conn));
                lock.unlock();

                self->Start();

            } catch (std::exception &e) {
                SPDLOG_ERROR("Exception: {}", e.what());
            }
        });
}

void Server::ClearSession(const std::string &session_id) {
    if (CheckValid(session_id)) {
        UserManager::GetInstance()->RemoveUserSession(
            _sessions[session_id]->GetUid());
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.erase(session_id);
    }
}

bool Server::CheckValid(const std::string &session_id) {
    auto it = _sessions.find(session_id);
    if (it == _sessions.end()) {
        return false;
    }
    return true;
}

void Server::on_timer(const boost::system::error_code &ec) {

    std::vector<std::string> expired_sessions;
    auto now = std::time(nullptr);
    int count = 0;
    {
        std::lock_guard<std::mutex> lock(_mutex);

        for (auto it = _sessions.begin(); it != _sessions.end(); ++it) {
            auto b_expired = it->second->IsHeartbeatExpired(now);
            if (b_expired) {
                expired_sessions.push_back(it->first);
                continue;
            }
        }
    }

    for (const auto &sid : expired_sessions) {
        auto it = _sessions.find(sid);
        if (it != _sessions.end()) {
            it->second->Close();
        }
    }

    _timer.expires_after(std::chrono::seconds(30));
    _timer.async_wait(
        [this](const boost::system::error_code &ec) { on_timer(ec); });
}

void Server::StopTimer() { _timer.cancel(); }
void Server::StartTimer() {
    _timer.async_wait(
        [self = shared_from_this()](const boost::system::error_code &ec) {
            self->on_timer(ec);
        });
    _timer_running = true;
}
