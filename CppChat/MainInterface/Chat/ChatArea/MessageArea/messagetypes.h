#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H


#include <QObject>
#include <QString>
#include <QDateTime>
#include <QUrl>
#include <QVariant>
#include <QUuid>
#include "../../../../usermanager.h"

enum class MessageType{
    TextMessage,
    ImageMessage,
    VideoMessage,
    AudioMessage,
    MixedMessage,
    OtherFileMessage
};

enum class MessageSource{
    Me = 0,
    Peer,   // 单独聊天
};

enum class MessageEnv{
    Private,
    Group
};

struct MessageContent{
    MessageType type;
    QVariant data;
    QString mimeType;
    int pos;            // 如果是类似图片+文字的混合类型需这个参数确定图片的位置;
};

struct MessageItem{
    QString recvId; // 接受者id
    MessageType type;
    QList<MessageContent>contents;  // 实际的内容串
    QString id; // 唯一的消息id
    QString senderId;   // 发送者的id
    QDateTime timestamp;    // 时间
    MessageEnv env;
    MessageSource from; // 自己还是对方发送的
    bool isSelected;  // 之后可能会有聊天记录的选择，删除
    MessageItem()
        :id(QUuid::createUuid().toString())
        ,senderId(UserManager::GetInstance()->GetName())
        ,timestamp(QDateTime::currentDateTime())
        ,env(MessageEnv::Private)
        ,from(MessageSource::Me)
        ,isSelected(false){}
};
Q_DECLARE_METATYPE(MessageContent)
Q_DECLARE_METATYPE(QList<MessageContent>)


#endif // MESSAGETYPES_H
