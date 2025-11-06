// ChatMessageModel.h
#ifndef CHATMESSAGEMODEL_H
#define CHATMESSAGEMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "ChatMessage.h"

class ChatMessageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CustomRoles {
        MessageIdRole = Qt::UserRole + 1,
        MessageTypeRole,
        MessageRoleRole,
        ContentRole,
        TimestampRole,
        SenderIdRole,
        SenderNameRole,
        MediaItemsRole,
        IsSelectedRole
    };

    explicit ChatMessageModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    void addMessage(const ChatMessage &message);
    void insertMessage(int index, const ChatMessage &message);
    void removeMessage(const QString &messageId);
    void clearMessages();
    void setMessagesSelected(const QVector<QString> &messageIds, bool selected);
    ChatMessage getMessage(const QString &messageId) const;

private:
    QVector<ChatMessage> m_messages;
};

#endif // CHATMESSAGEMODEL_H
