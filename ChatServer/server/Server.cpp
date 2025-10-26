#include "Server.h"
#include "../session/Session.h"
#include "AsioPool.h"
#include <spdlog/spdlog.h>

Server::Server(net::io_context& ioc, uint16_t port)
    : _ioc(ioc)
    , _acceptor(ioc, net::ip::tcp::endpoint(net::ip::tcp::v4(), port))
    , _port(port)
{
    SPDLOG_INFO("Server Start Success,Listen on port:{}", _port);
}

void Server::Start()
{
    auto& io_context = AsioPool::GetInstance()->GetIOService();
    std::shared_ptr<Session> conn = std::make_shared<Session>(io_context, this);
    _acceptor.async_accept(conn->GetSocket(), [this, conn, self = shared_from_this()](const boost::system::error_code& ec) {
        try {
            if (ec) {
                self->Start();
                return;
            }
            conn->Start();
            SPDLOG_INFO("New connection from {},session:{}", conn->GetSocket().remote_endpoint().address().to_string(),conn->GetSessionId());

            std::unique_lock<std::mutex> lock(_mutex);
            _sessions.insert(std::make_pair(conn->GetSessionId(), conn));
            lock.unlock();

            self->Start();
        } catch (std::exception& e) {
            SPDLOG_ERROR("Exception: {}", e.what());
        }
    });
}

void Server::ClearSession(const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions.erase(session_id);
}

bool Server::CheckValid(const std::string& session_id)
{
    auto it = _sessions.find(session_id);
    if (it == _sessions.end()) {
        return false;
    }
    return true;
}
