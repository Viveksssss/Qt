#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include "Properties/singleton.h"
#include <QObject>
#include <memory>
#include <QTcpSocket>
#include <QHash>
#include <functional>
class TcpManager
    : public QObject
    , public Singleton<TcpManager>
    , public std::enable_shared_from_this<TcpManager>
{
    Q_OBJECT
    friend class Singleton<TcpManager>;
public:
    ~TcpManager();

private:
    TcpManager();
    // 注册回调
    void initHandlers();
    // 连接
    void connections();
    // 处理单个数据->hash找回调处理
    void handleMessage(RequestType requestType,int len,QByteArray data);
private:
    // 连接实例
    QTcpSocket _socket;
    // 地址
    QString _host;
    // 端口
    uint16_t _port;
    // 缓存容器
    QByteArray _buffer;
    // 处理包头
    bool _recv_pending;
    // Type
    quint16 _msg_id;
    // Length
    quint16 _msg_len;
    // 存放请求和对应的回调函数
    QHash<RequestType,std::function<void(RequestType,int,QByteArray)>>_handlers;
public slots:
    void do_tcp_connect(ServerInfo); // from LoginScreen::on_tcp_connect
    void do_send_data(RequestType requestType,QByteArray data); // from TcpManager::to_send_data
signals:
    void on_connect_success(bool success); // to LoginScreen::do_connect_success
    void on_send_data(RequestType requestType,QByteArray data); // to TcpManager::do_send_data
    void on_switch_interface(); // to MainWindow::do
    void on_login_failed(int);  // to LoginScreen::do_login_failed
    void on_users_searched(QList<std::shared_ptr<UserInfo>>list);   // to AnimatedSearchBox::do_users_searched
    void on_add_friend(UserInfo&&info,bool ok); // to SideNews::do_add_friend
};

#endif // TCPMANAGER_H
