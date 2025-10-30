#ifndef FRIENDITEM_H
#define FRIENDITEM_H


// FriendItem.h
#include <QString>

struct FriendItem {
    QString id;          // 好友ID
    QString name;        // 昵称
    QString avatar;      // 头像路径
    QString status;      // 在线状态：在线、离线、忙碌等
    QString message;     // 最新的部分消息

    FriendItem(const QString &id, const QString &name,
               const QString &avatar = "", const QString &status = "离线",
               const QString &message = "")
        : id(id)
        , name(name)
        , avatar(avatar)
        , status(status)
        , message(message)
    {}
};





#endif // FRIENDITEM_H
