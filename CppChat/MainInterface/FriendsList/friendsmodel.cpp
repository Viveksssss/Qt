#include "friendsmodel.h"

FriendsModel::FriendsModel(QObject *parent)
    : QAbstractListModel{parent}
{}

int FriendsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _friends.size();
}

QVariant FriendsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _friends.size()){
        return QVariant();
    }

    const FriendItem&friendItem = _friends.at(index.row());

    switch(role){
    case Qt::DisplayRole:
        return QString("%1(%2)").arg(friendItem.name).arg(friendItem.status);
    case NameRole:
        return friendItem.name;
    case IdRole:
        return friendItem.id;
    case AvatarRole:
        return friendItem.avatar;
    case StatusRole:
        return friendItem.status;
    case MessageRole:
        return friendItem.message;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FriendsModel::roleNames() const
{
    QHash<int ,QByteArray>roles;
    roles[IdRole] = "friendId";
    roles[NameRole] = "friendName";
    roles[AvatarRole] = "avatar";
    roles[StatusRole] = "status";
    roles[MessageRole] = "message";
    return roles;
}

void FriendsModel::addFriend(const FriendItem &friendItem)
{
    beginInsertRows(QModelIndex(),_friends.size(),_friends.size());
    _friends.append(friendItem);
    endInsertRows();
}

FriendItem FriendsModel::getFriend(int index)
{
    if (index >= 0 && index < _friends.size()){
        return _friends.at(index);
    }
    return FriendItem("","");
}
