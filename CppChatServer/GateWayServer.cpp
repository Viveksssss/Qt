#include "GateWayServer.h"
#include "Session.h"
#include <iostream>

GateWayServer::GateWayServer(net::io_context& ioc, unsigned short port)
    : _ioc(ioc)
    , _acceptor(ioc, net::ip::tcp::endpoint(net::ip::tcp::v4(), port))
    , _socket(ioc)
{
}

void GateWayServer::Start()
{
    _acceptor.async_accept(_socket, [this, self = shared_from_this()](const boost::system::error_code& ec) {
        try {
            if (ec) {
                self->Start();
                return;
            }
            std::make_shared<Session>(std::move(self->_socket))->Start();
            self->Start();
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    });
}
