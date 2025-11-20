#ifndef FRIENDITEM_H
#define FRIENDITEM_H


// FriendItem.h
#include <QString>
#include "../Properties/global.h"

struct FriendItem : public UserInfo {
    QString message;     // 最新的部分消息

    explicit FriendItem(int id,int status,int sex,const QString &name = "",
               const QString &avatar = "",
               const QString &message = "")

        : message(message)  // 如果是消息列表message就是最近的消息，如果是好友列表就是个性签名
    {
        this->status = status;
        this->id = id;
        this->name = name;
        this->avatar = avatar;
        this->sex = sex;
    }
    explicit FriendItem(){}
};





#endif // FRIENDITEM_H
