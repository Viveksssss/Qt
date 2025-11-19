#include "tcpmanager.h"
#include "usermanager.h"
#include <QAbstractSocket>
#include <QDataStream>
#include <QJsonObject>
#include <QDir>
#include <QJsonArray>
#include <QMessageBox>

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
    /**
     * @brief 用户登录请求回包处理
     */
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

        // 基本信息
        UserManager::GetInstance()->SetName(jsonObj["name"].toString());
        UserManager::GetInstance()->SetEmail(jsonObj["email"].toString());
        UserManager::GetInstance()->SetToken(jsonObj["token"].toString());
        UserManager::GetInstance()->SetIcon(jsonObj["icon"].toString());
        UserManager::GetInstance()->SetUid(jsonObj["uid"].toInt());
        qDebug() << "asdasdasdasdasd \t" << UserManager::GetInstance()->GetUid();
        UserManager::GetInstance()->SetSex(jsonObj["sex"].toInt());
        UserManager::GetInstance()->SetStatus(1);

        qDebug() << "reday to apply_list";


        // 申请列表
        if (jsonObj.contains("apply_friends")){
            QJsonArray apply_friends = jsonObj["apply_friends"].toArray();
            std::vector<std::shared_ptr<UserInfo>>apply_list;
            for(const QJsonValue&value:apply_friends){
                QJsonObject obj = value.toObject();
                auto user_info = std::make_shared<UserInfo>();
                user_info->id = obj["uid"].toInt();
                user_info->name = obj["name"].toString();
                user_info->email = obj["email"].toString();
                user_info->avatar = obj["icon"].toString();
                user_info->sex = obj["sex"].toInt();
                user_info->desc = obj["desc"].toString();
                user_info->back = obj["back"].toString();//备用字段这里存放时间
                apply_list.push_back(user_info);
            }
            emit on_get_apply_list(apply_list);
        }

        //TODO: 通知列表
        if(jsonObj.contains("notifications")){
            QJsonArray notification_array = jsonObj["notifications"].toArray();
            std::vector<std::shared_ptr<UserInfo>>notification_list;
            for(const QJsonValue&value:notification_array){
                QJsonObject obj = value.toObject();
                auto user_info = std::make_shared<UserInfo>();
                user_info->id = obj["uid"].toInt();
                user_info->status = obj["type"].toInt();
                user_info->desc = obj["message"].toString();
                user_info->back = obj["time"].toString();
                user_info->avatar = obj["icon"].toString();
                notification_list.push_back(user_info);
            }
            emit on_message_to_list(notification_list);
        }

        //TODO: 好友列表
        if (jsonObj.contains("friends")){
            QJsonArray friend_array = jsonObj["friends"].toArray();
            std::vector<std::shared_ptr<UserInfo>>friends_list;
            for(const QJsonValue&value:friend_array){
                QJsonObject obj = value.toObject();
                auto user_info = std::make_shared<UserInfo>();
                user_info->id = obj["uid"].toInt();
                user_info->sex = obj["sex"].toInt();
                user_info->status = obj["status"].toInt();
                user_info->name = obj["name"].toString();
                user_info->email = obj["email"].toString();
                user_info->avatar = obj["icon"].toString();

                friends_list.push_back(user_info);
            }
            emit on_add_friends_to_list(friends_list);
        }

        //TODO: 消息列表

        // 发出信号跳转到主页面
        emit on_switch_interface();
    };

    /**
     * @brief 用户搜索回包处理
     */
    _handlers[RequestType::ID_SEARCH_USER_RSP] = [this](RequestType requestType,int len,QByteArray data){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()){
            qDebug() << "Error occured about Json";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")){
            int err = static_cast<int>(ErrorCodes::ERROR_JSON);
            qDebug() << "Search Failed,Error Is Json Parse Error " <<err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(ErrorCodes::SUCCESS)){
            qDebug() << "Search Failed,Error Is " << err;
            return;
        }

        // 解析查询的用户列表
        QList<std::shared_ptr<FriendInfo>> userList;
        if (jsonObj.contains("users") && jsonObj["users"].isArray()) {
            QJsonArray usersArray = jsonObj["users"].toArray();

            for (const QJsonValue &userValue : usersArray) {
                if (userValue.isObject()) {
                    QJsonObject userObj = userValue.toObject();

                    QString avatar;
                    if (userObj.contains("icon")&&userObj["icon"]!="NULL"&&!userObj["icon"].isNull()) {
                        QString base64Avatar = userObj["icon"].toString();
                        QByteArray avatarData = QByteArray::fromBase64(base64Avatar.toUtf8());
                        avatar = avatarData;
                    } else {
                        // 没有头像字段，使用默认头像
                        avatar = ":/Resources/main/header-default.png";
                    }

                    int id = userObj["uid"].toInt();
                    int status = userObj["status"].toInt();
                    int sex = userObj["sex"].toInt();
                    // QString email = userObj["email"].toString();
                    QString name = userObj["name"].toString();
                    bool isFriend = userObj["isFriend"].toBool();
                    auto user_info = std::make_shared<FriendInfo>(id,avatar,name,sex,status,isFriend);

                    userList.append(user_info);
                }
            }

        }
        if(userList.count() == 0){
            return;
        }
        emit on_users_searched(userList);
    };

    /**
     * @brief 用户添加请求回包处理
     */
    _handlers[RequestType::ID_ADD_FRIEND_RSP] = [this](RequestType requestType,int len,QByteArray data){
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
      UserInfo info;
      info.id = jsonObj["fromUid"].toInt();
      // TODO:
      qDebug() << "申请添加好友成功";
      emit on_add_friend(info);
  };

    /**
     * @brief 用户请求添加好友通知处理
     */
    _handlers[RequestType::ID_NOTIFY_ADD_FRIEND_REQ] = [this](RequestType requestType,int len,QByteArray data){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()){
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")){
            int err = static_cast<int>(ErrorCodes::ERROR_JSON);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(ErrorCodes::SUCCESS)){
            return;
        }

        int from_uid = jsonObj["from_uid"].toInt();
        int from_sex = jsonObj["from_sex"].toInt();
        QString from_name = jsonObj["from_name"].toString();
        QString from_icon = jsonObj["from_icon"].toString();
        QString from_desc = jsonObj["from_desc"].toString();

        auto user_info = std::make_shared<UserInfo>();
        user_info->id = from_uid;
        user_info->sex = from_sex;
        user_info->name = from_name;
        user_info->avatar = from_icon;
        user_info->desc = from_desc;

        emit on_auth_friend(user_info);
    };

    /**
     * @brief 用户请求添加好友通知回包
     */
    _handlers[RequestType::ID_AUTH_FRIEND_RSP] = [this](RequestType requestType,int len,QByteArray data){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()){
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")){
            int err = static_cast<int>(ErrorCodes::ERROR_JSON);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(ErrorCodes::SUCCESS)){
            return;
        }
        // 接受信息如果成功，然后将对方信息加入好友列表
        if (jsonObj.contains("ok") && jsonObj["ok"].toBool()){
            auto info = std::make_shared<UserInfo>();
            info->id = jsonObj["to_uid"].toInt();
            info->status = jsonObj["to_status"].toInt();
            info->sex = jsonObj["to_sex"].toInt();
            info->name = jsonObj["to_name"].toString();
            info->avatar = jsonObj["to_icon"].toString();
            info->desc = jsonObj["to_desc"].toString();
            info->back = jsonObj["to_message"].toString();
            if (jsonObj["accept"].toBool()){
                emit on_add_friend_to_list(info);
            }
            emit on_notify_friend(info,jsonObj["accept"].toBool());
        }else{
            //TODO: 暂时忽略
        }
    };

    _handlers[RequestType::ID_NOTIFY_AUTH_FRIEND_REQ] = [this](RequestType requestType,int len,QByteArray data){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()){
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")){
            int err = static_cast<int>(ErrorCodes::ERROR_JSON);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(ErrorCodes::SUCCESS)){
            return;
        }

        auto info = std::make_shared<UserInfo>();
        info->id = jsonObj["from_uid"].toInt();
        info->name = jsonObj["from_name"].toString();
        info->sex = jsonObj["from_sex"].toInt();
        info->avatar = jsonObj["from_icon"].toString();
        info->status = jsonObj["from_status"].toInt();
        info->desc = jsonObj["message"].toString();     // 临时存放消息
        if (jsonObj["type"].toInt() == static_cast<int>(NotificationCodes::ID_NOTIFY_MAKE_FRIENDS)){
            emit on_add_friend_to_list(info);
            emit on_notify_friend2(info,true);
        }else{
            emit on_notify_friend2(info,false);
        }
    };

    /**
     * @brief 通知
    */
    _handlers[RequestType::ID_NOTIFY] = [this](RequestType requestType,int len,QByteArray data){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()){
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")){
            int err = static_cast<int>(ErrorCodes::ERROR_JSON);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(ErrorCodes::SUCCESS)){
            return;
        }

        auto user_info = std::make_shared<UserInfo>();
        user_info->id = jsonDoc["uid"].toInt();
        user_info->status = jsonDoc["type"].toInt();
        user_info->desc = jsonDoc["message"].toString();
        user_info->back = jsonDoc["time"].toString();
        user_info->avatar = jsonDoc["icon"].toString();
        std::vector<std::shared_ptr<UserInfo>>vec;
        vec.push_back(user_info);
        emit on_message_to_list(vec);
        emit on_change_friend_status(user_info->id,1);
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
            // 在TLV解析的最开始添加
            qDebug() << "=== 收到TLV消息 ===";
            qDebug() << "Type:" << static_cast<int>(_msg_id);
            qDebug() << "Length:" << _msg_len;
            qDebug() << "Data:" << msgBody;
            qDebug() << "===================";
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
        QMessageBox msgBox;
        msgBox.setWindowTitle("Network Issue");
        msgBox.setText("No Connection to Server");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);

        // macOS 风格样式表
        msgBox.setStyleSheet(R"(
            QMessageBox {
                background-color: #f5f5f7;
                border: 1px solid #d0d0d0;
                border-radius: 10px;
                font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
            }
            QMessageBox QLabel {
                color: #1d1d1f;
                font-size: 14px;
                font-weight: 400;
                padding: 15px;
            }
            QMessageBox QLabel#qt_msgbox_label {
                min-width: 300px;
            }
            QMessageBox QPushButton {
                background-color: #007aff;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 8px 24px;
                font-size: 13px;
                font-weight: 500;
                min-width: 80px;
                margin: 5px;
            }
            QMessageBox QPushButton:hover {
                background-color: #0056d6;
            }
            QMessageBox QPushButton:pressed {
                background-color: #0040a8;
            }
            QMessageBox QPushButton:focus {
                outline: 2px solid #007aff;
                outline-offset: 2px;
            }
        )");

        msgBox.exec();
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
