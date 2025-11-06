// ChatMessageModel.cpp
#include "chatmessagemodel.h"

ChatMessageModel::ChatMessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ChatMessageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_messages.size();
}

QVariant ChatMessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return QVariant();

    const ChatMessage &message = m_messages.at(index.row());

    switch (role) {
    case MessageIdRole:
        return message.id();
    case MessageTypeRole:
        return static_cast<int>(message.type());
    case MessageRoleRole:
        return static_cast<int>(message.role());
    case ContentRole:
        return message.content();
    case TimestampRole:
        return message.timestamp();
    case SenderIdRole:
        return message.senderId();
    case SenderNameRole:
        return message.senderName();
    case MediaItemsRole:
        return QVariant::fromValue(message.mediaItems());
    case IsSelectedRole:
        return message.isSelected();
    default:
        return QVariant();
    }
}

bool ChatMessageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return false;

    ChatMessage &message = m_messages[index.row()];

    switch (role) {
    case IsSelectedRole:
        message.setSelected(value.toBool());
        emit dataChanged(index, index, {role});
        return true;
    default:
        return false;
    }
}

QHash<int, QByteArray> ChatMessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageIdRole] = "messageId";
    roles[MessageTypeRole] = "messageType";
    roles[MessageRoleRole] = "messageRole";
    roles[ContentRole] = "content";
    roles[TimestampRole] = "timestamp";
    roles[SenderIdRole] = "senderId";
    roles[SenderNameRole] = "senderName";
    roles[MediaItemsRole] = "mediaItems";
    roles[IsSelectedRole] = "isSelected";
    return roles;
}

void ChatMessageModel::addMessage(const ChatMessage &message)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.append(message);
    endInsertRows();
}

void ChatMessageModel::insertMessage(int index, const ChatMessage &message)
{
    beginInsertRows(QModelIndex(), index, index);
    m_messages.insert(index, message);
    endInsertRows();
}

void ChatMessageModel::removeMessage(const QString &messageId)
{
    for (int i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i].id() == messageId) {
            beginRemoveRows(QModelIndex(), i, i);
            m_messages.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

void ChatMessageModel::clearMessages()
{
    beginResetModel();
    m_messages.clear();
    endResetModel();
}

void ChatMessageModel::setMessagesSelected(const QVector<QString> &messageIds, bool selected)
{
    for (int i = 0; i < m_messages.size(); ++i) {
        if (messageIds.contains(m_messages[i].id())) {
            m_messages[i].setSelected(selected);
            QModelIndex idx = createIndex(i, 0);
            emit dataChanged(idx, idx, {IsSelectedRole});
        }
    }
}

ChatMessage ChatMessageModel::getMessage(const QString &messageId) const
{
    for (const auto &message : m_messages) {
        if (message.id() == messageId) {
            return message;
        }
    }
    return ChatMessage();
}
