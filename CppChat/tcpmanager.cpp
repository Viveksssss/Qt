#include "tcpmanager.h"
#include "usermanager.h"
#include <QAbstractSocket>
#include <QDataStream>
#include <QJsonObject>
#include <QDir>
#include <QJsonArray>

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

        UserManager::GetInstance()->SetName(jsonObj["name"].toString());
        UserManager::GetInstance()->SetEmail(jsonObj["email"].toString());
        UserManager::GetInstance()->SetToken(jsonObj["token"].toString());

        // 发出信号跳转到主页面
        emit on_switch_interface();
    };


    _handlers[RequestType::ID_SEARCH_USER_RSP] = [this](RequestType requestType,int len,QByteArray data){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()){
            qDebug() << "Error occured about Json";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")){
            int err = static_cast<int>(ErrorCodes::ERROR_JSON);
            qDebug() << "AddFriend Failed,Error Is Json Parse Error " <<err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(ErrorCodes::SUCCESS)){
            qDebug() << "AddFriend Failed,Error Is " << err;
            return;
        }

        // 解析查询的用户列表
        QList<std::shared_ptr<UserInfo>> userList;
        if (jsonObj.contains("users") && jsonObj["users"].isArray()) {
            QJsonArray usersArray = jsonObj["users"].toArray();

            for (const QJsonValue &userValue : usersArray) {
                if (userValue.isObject()) {
                    QJsonObject userObj = userValue.toObject();

                    QPixmap avatar;
                    QString tempFilePath;
                    if (userObj.contains("avatar")) {
                        QString base64Avatar = userObj["avatar"].toString();
                        QByteArray avatarData = QByteArray::fromBase64(base64Avatar.toUtf8());
                        avatar.loadFromData(avatarData);
                        tempFilePath = QDir::tempPath() + "/tmp_from_quick_chat_image_" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".png";
                        // 如果加载失败，使用默认头像
                        if (avatar.isNull()) {
                            avatar = QPixmap(":/Resources/main/header.png");
                        }
                    } else {
                        // 没有头像字段，使用默认头像
                        avatar = QPixmap(":/Resources/main/header.png");
                    }

                    QString id = userObj["uid"].toString();
                    QString email = userObj["email"].toString();
                    QString name = userObj["name"].toString();
                    QString status = userObj["status"].toString();
                    QString avatar_path = tempFilePath;
                    auto user_info = std::make_shared<UserInfo>(id,email,name,avatar_path,status);

                    userList.append(user_info);
                }
            }

        }
        if(userList.count() == 0){
            return;
        }
        emit on_users_searched(userList);
    };

    // TODO:
    _handlers[RequestType::ID_ADD_FRIEND_REQ] = [this](RequestType requestType,int len,QByteArray data){
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
        //_____________________________;
  };
}

void TcpManager::connections()
{
    // 连接成功
    connect(&_socket,&QTcpSocket::connected,[&](){
        qDebug() << "Connected to Server " << _socket.peerAddress().toString() << ":" << _socket.peerPort();
        emit on_connect_success(true);
    });
    // 读取数据
    connect(&_socket,&QTcpSocket::readyRead,[&](){
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer,QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        qDebug() << _buffer;
        forever{
            if (!_recv_pending){
                if (_buffer.size() < static_cast<int>(sizeof(qint16)*2)){
                    return;
                }
                stream >> _msg_id >> _msg_len;
            }
            if (_buffer.size() < _msg_len){
                _recv_pending = true;
                return;
            }
            _recv_pending = false;
            QByteArray msgBody = _buffer.mid(4,_msg_len);
            _buffer.remove(0,4+_msg_len);
            handleMessage(RequestType(_msg_id),_msg_len,msgBody);
        }
    });
    // 错误
    connect(&_socket,&QTcpSocket::errorOccurred,[&](QTcpSocket::SocketError socketError){
        qDebug() << "Socket Error["<<socketError<< "]:" << _socket.errorString();
        emit on_login_failed(static_cast<int>(ErrorCodes::ERROR_NETWORK));
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
    qDebug() << "Connecting to server " << si.host << ":" << si.port ;
    _host = si.host;
    _port = static_cast<uint16_t>(si.port.toInt());
    _socket.connectToHost(_host,_port);
}

void TcpManager::do_send_data(RequestType requestType, QByteArray data)
{

    if (!_socket.isOpen()){
        qDebug() << "No Connection!";
        return;
    }
    auto id =static_cast<uint16_t>(requestType);

    quint16 len = static_cast<quint16>(data.size());

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out.setByteOrder(QDataStream::BigEndian);

    out << id << len;

    block.append(data);

    _socket.write(block);
}
