#ifndef FRIENDSMODEL_H
#define FRIENDSMODEL_H

#include "frienditem.h"

#include <QAbstractListModel>
#include <QObject>

class FriendsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum FriendRole{
        IdRole = Qt::UserRole + 1,  // id
        NameRole,                   // 昵称
        AvatarRole,                 // 头像
        StatusRole,                 // 状态
        MessageRole,                // 最近消息
    };


    explicit FriendsModel(QObject *parent = nullptr);
    // QAbstractItemModel interface
    int rowCount(const QModelIndex&parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray>roleNames()const override;
    void addFriend(const FriendItem&friendItem);
    FriendItem getFriend(int index);
private:
    QVector<FriendItem>_friends;
};

#endif // FRIENDSMODEL_H
