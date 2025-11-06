// ChatMessage.h
#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QVector>
#include "MessageType.h"

struct MediaItem {
    QString type;  // "text", "image", "video", "audio"
    QString content;
    QString filePath;
    QVariant data;
};

class ChatMessage
{
public:
    ChatMessage(){}

    QString id() const { return m_id; }
    void setId(const QString &id) { m_id = id; }

    MessageType type() const { return m_type; }
    void setType(MessageType type) { m_type = type; }

    MessageRole role() const { return m_role; }
    void setRole(MessageRole role) { m_role = role; }

    QString content() const { return m_content; }
    void setContent(const QString &content) { m_content = content; }

    QDateTime timestamp() const { return m_timestamp; }
    void setTimestamp(const QDateTime &timestamp) { m_timestamp = timestamp; }

    QString senderId() const { return m_senderId; }
    void setSenderId(const QString &senderId) { m_senderId = senderId; }

    QString senderName() const { return m_senderName; }
    void setSenderName(const QString &senderName) { m_senderName = senderName; }

    QVector<MediaItem> mediaItems() const { return m_mediaItems; }
    void setMediaItems(const QVector<MediaItem> &items) { m_mediaItems = items; }
    void addMediaItem(const MediaItem &item) { m_mediaItems.append(item); }

    bool isSelected() const { return m_selected; }
    void setSelected(bool selected) { m_selected = selected; }

private:
    QString m_id;
    MessageType m_type;
    MessageRole m_role;
    QString m_content;
    QDateTime m_timestamp;
    QString m_senderId;
    QString m_senderName;
    QVector<MediaItem> m_mediaItems;
    bool m_selected = false;
};

#endif // CHATMESSAGE_H
