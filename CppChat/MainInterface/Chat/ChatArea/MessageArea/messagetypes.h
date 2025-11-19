#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H


#include <QObject>
#include <QString>
#include <QDateTime>
#include <QUrl>
#include <QVariant>
#include <QUuid>
#include "../../../../usermanager.h"
#include "../../../../proto/im.pb.h"

enum class MessageType{
    TextMessage,
    ImageMessage,
    VideoMessage,
    AudioMessage,
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
    MessageType     type;           // 自定义类型
    QVariant        data;           // 如果是文本文件，存放在这里，如果是二进制，此为空。
    QString         mimeType;       // 具体的类型比如text/plain
    QString         fid;            // 文件服务器需要用
};

struct MessageItem{
    QString               id;           // 唯一的消息id
    int                   to_id;        // 接受者id
    int                   from_id;      // 发送者的id
    QDateTime             timestamp;    // 时间
    MessageSource         from;         // 自己还是对方发送的
    MessageEnv            env;          // 私聊还是群聊
    MessageContent        content;      // 实际的内容串
    bool                  isSelected;   // 之后可能会有聊天记录的选择，删除

    MessageItem()
        :id(QUuid::createUuid().toString())
        ,from_id(UserManager::GetInstance()->GetUid())
        ,timestamp(QDateTime::currentDateTime())
        ,env(MessageEnv::Private)
        ,from(MessageSource::Me)
        ,isSelected(false){}
};

// 转成发给服务器的im::MessageItem
static im::MessageItem toPb(const MessageItem &m)
{
    im::MessageItem pb;
    pb.set_id(m.id.toStdString());
    pb.set_from_id(m.from_id);
    pb.set_to_id(m.to_id);
    pb.set_timestamp(m.timestamp.toMSecsSinceEpoch());
    pb.set_env(static_cast<int32_t>(m.env));


    auto* c = pb.mutable_content();
    c->set_type(static_cast<int32_t>(m.content.type));
    c->set_data(m.content.data.toString().toStdString());
    c->set_mime_type(m.content.mimeType.toStdString());
    c->set_fid(m.content.fid.toStdString());
    return pb;
}

// 服务器收回来解析成MessageItem
static MessageItem fromPb(const im::MessageItem&pb)
{
    MessageItem m;
    m.id                = QString::fromStdString(pb.id());
    m.to_id             = pb.to_id();
    m.from_id           = pb.from_id();
    m.timestamp         = QDateTime::fromMSecsSinceEpoch(pb.timestamp());
    m.env               = MessageEnv(pb.env());
    m.from              = MessageSource::Peer;
    m.content.fid       = QString::fromStdString(pb.content().fid());
    m.content.type      = MessageType(pb.content().type());
    m.content.data      = QString::fromStdString(pb.content().data());
    m.content.mimeType  = QString::fromStdString(pb.content().mime_type());
    return m;
}

Q_DECLARE_METATYPE(MessageContent)
Q_DECLARE_METATYPE(QList<MessageContent>)


#endif // MESSAGETYPES_H
