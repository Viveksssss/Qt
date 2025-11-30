#ifndef GATEWAYSERVER_H
#define GATEWAYSERVER_H

#include "../session/Session.h"
#include <memory>
#include <mutex>
#include <unordered_map>

class Server : public std::enable_shared_from_this<Server> {
public:
    Server(net::io_context& ioc, uint16_t port);
    ~Server();
    void Start();
    void ClearSession(const std::string& session_id);
    bool CheckValid(const std::string& session_id);

    std::string GetServerName() const { return _server_name; }

private:
    net::ip::tcp::acceptor _acceptor;
    net::io_context& _ioc;
    uint16_t _port;
    std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;
    std::mutex _mutex;
    std::string _server_name;
};

#endif
