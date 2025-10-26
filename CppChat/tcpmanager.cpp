#include "tcpmanager.h"
#include <QAbstractSocket>
#include <QDataStream>
#include <QJsonObject>

TcpManager::~TcpManager() = default;

TcpManager::TcpManager()
    : _host("")
    , _port(0)
    , _recv_pending(false)
    , _msg_id(0)
    , _msg_len(0)
{
    initHandlers();
    connections();
}

void TcpManager::initHandlers()
{
    _handlers[RequestType::ID_CHAT_LOGIN_RSP] = [this](RequestType requestType,int len,QByteArray data){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()){
            qDebug() << "Error occured about Json";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")){
            int err = static_cast<int>(ErrorCodes::ERROR_JSON);
            qDebug() << "Login Failed,Error Is Json Parse Error " <<err;
            emit on_login_failed(err);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(ErrorCodes::SUCCESS)){
            qDebug() << "Login Failed,Error Is " << err;
            emit on_login_failed(err);
            return;
        }

        emit on_switch_interface();
    };
}

void TcpManager::connections()
{
    // 连接成功
    connect(&_socket,&QTcpSocket::connected,[&](){
        qDebug() << "Connected to Server.";
        emit on_connect_success(true);
    });
    // 读取数据
    connect(&_socket,&QTcpSocket::readyRead,[&](){
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer,QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        forever{
            if (!_recv_pending){
                if (_buffer.size() < static_cast<int>(sizeof(qint16)*2)){
                    return;
                }
                stream >> _msg_id >> _msg_len;
                _buffer.remove(0,4);
                qDebug() << "Message Id:" << _msg_id << " len:" << _msg_len ;
            }

            if (_buffer.size() < _msg_len){
                _recv_pending = true;
                return;
            }
            _recv_pending = false;
            QByteArray msgBody = _buffer.mid(_msg_len);
            qDebug() << "Receive body:" <<msgBody;
            _buffer.remove(0,_msg_len);
            handleMessage(RequestType(_msg_id),_msg_len,msgBody);
        }
    });
    // 错误
    connect(&_socket,&QTcpSocket::errorOccurred,[&](QTcpSocket::SocketError socketError){
        qDebug() << "Socket Error["<<socketError<< "]:" << _socket.errorString();
    });
    // 断开连接
    connect(&_socket,&QTcpSocket::disconnected,[&](){
        qDebug() << "Disconnected from server - " << _socket.peerAddress().toString() <<":"<<_socket.peerPort();
    });
    // 发送数据
    connect(this,&TcpManager::on_send_data,this,&TcpManager::do_send_data);
}

void TcpManager::handleMessage(RequestType requestType, int len, QByteArray data)
{
    auto it = _handlers.find(requestType);
    if ( it == _handlers.end()){
        qDebug() <<  "Not Found[" << static_cast<int>(requestType) << "] to handle";
        return;
    }
    it.value()(requestType,len,data);
}

void TcpManager::do_tcp_connect(ServerInfo si)
{
    qDebug() << "Connecting to server...";
    _host = si.host;
    _port = static_cast<uint16_t>(si.port.toInt());
    _socket.connectToHost(_host,_port);
}

void TcpManager::do_send_data(RequestType requestType, QString data)
{
    auto id =static_cast<uint16_t>(requestType);

    QByteArray dataBytes = data.toUtf8();

    quint16 len = static_cast<quint16>(data.size());

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out.setByteOrder(QDataStream::BigEndian);

    out << id << len;

    block.append(dataBytes);

    _socket.write(block);
}
