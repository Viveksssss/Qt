#include "messagesmodel.h"

#include <QLabel>


MessagesModel::MessagesModel(QObject *parent)
    : QAbstractListModel{parent}
{}

int MessagesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _messages.size();
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _messages.size()){
        return QVariant();
    }

    const ConversationItem&messageItem = _messages.at(index.row());

    switch(role){
    case Qt::DisplayRole:
        return QString("%1(%2)").arg(messageItem.name).arg(messageItem.status);
    case NameRole:
        return messageItem.name;
    case IdRole:
        return messageItem.id;
    case ToUidRole:
        return messageItem.to_uid;
    case AvatarRole:
        return messageItem.icon;
    case StatusRole:
        return messageItem.status;
    case MessageRole:
        return messageItem.message;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    QHash<int ,QByteArray>roles;
    roles[IdRole] = "messageId";
    roles[ToUidRole] = "toUid";
    roles[NameRole] = "messageName";
    roles[AvatarRole] = "avatar";
    roles[StatusRole] = "status";
    roles[MessageRole] = "message";
    return roles;
}

void MessagesModel::addMessage(const ConversationItem &messageItem)
{
    beginInsertRows(QModelIndex(),_messages.size(),_messages.size());
    _messages.append(messageItem);
    endInsertRows();
}

void MessagesModel::addPreMessage(const ConversationItem &messageItem)
{
    beginInsertRows(QModelIndex(),0,0);
    _messages.append(messageItem);
    endInsertRows();
}

ConversationItem MessagesModel::getMessage(int index)
{
    if (index >= 0 && index < _messages.size()){
        return _messages.at(index);
    }
    return ConversationItem();
}

QModelIndex MessagesModel::indexFromUid(int uid) const
{
    // 遍历所有行，检查uid角色
    for (int row = 0; row < rowCount(); ++row) {
        QModelIndex index = createIndex(row, 0);
        if (data(index, ToUidRole).toInt() == uid) {
            return index;
        }
    }
    return QModelIndex();
}

QVector<ConversationItem> &MessagesModel::getList()
{
    return this->_messages;
}

bool MessagesModel::existMessage(int uid)
{
    auto it = std::find_if(_messages.begin(),_messages.end(),[uid](const ConversationItem&item){
        return item.to_uid == uid;
    });
    return it == _messages.end() ? false : true;
}

bool MessagesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || row > _messages.size()){
        return false;
    }
    beginRemoveRows(parent,row,row+count-1);
    _messages.remove(row,count);
    endRemoveRows();
    return true;
}

bool MessagesModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent.isValid() || destinationParent.isValid() || count != 1){
        return false;
    }
    if (sourceRow == destinationChild){
        return false;
    }
    if (!(sourceRow >= 0 &&sourceRow <= _messages.size() &&destinationChild>=0 && destinationChild<=_messages.size())){
        return false;
    }

    beginMoveRows(sourceParent,sourceRow,sourceRow+count-1,destinationParent,destinationChild);
    if (sourceRow < destinationChild){
        _messages.insert(destinationChild,_messages.at(sourceRow));
    }else{
        _messages.insert(destinationChild, _messages.at(sourceRow));
        _messages.remove(sourceRow + 1);   // 因为刚插完index+1
    }
    endMoveRows();
    return true;
}

bool MessagesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= _messages.size()){
        return false;
    }

    ConversationItem& messageItem = _messages[index.row()];

    switch(role){
    case Qt::DisplayRole:
        break;
    case NameRole:
        messageItem.name = value.toString();
        break;
    case ToUidRole:
        messageItem.to_uid = value.toInt();
        break;
    case IdRole:
        // messageItem.id = value.toInt();
        break;
    case AvatarRole:
        messageItem.icon = value.toString();
        break;
    case StatusRole:
        messageItem.status = value.toInt();
        break;
    case MessageRole:
        messageItem.message = value.toString();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

