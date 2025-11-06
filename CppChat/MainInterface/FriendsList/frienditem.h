#ifndef FRIENDITEM_H
#define FRIENDITEM_H


// FriendItem.h
#include <QString>
#include "../../Properties/global.h"

struct FriendItem:public UserInfo {
    QString status;      // 在线状态：在线、离线、忙碌等
    QString message;     // 最新的部分消息

    FriendItem(const QString &id, const QString &name,
               const QString &avatar = "", const QString &status = "离线",
               const QString &message = "")

        : status(status)
        , message(message)
    {
        this->id = id;
        this->name = name;
        this->avatar = avatar;
    }
};





#endif // FRIENDITEM_H
