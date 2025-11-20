#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include "../FriendsList/frienditem.h"

#include <QAbstractListModel>
#include <QObject>


class MessagesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum MessageRole{
        IdRole = Qt::UserRole + 1,  // id
        NameRole,                   // 昵称
        AvatarRole,                 // 头像
        StatusRole,                 // 状态
        MessageRole,                // 最近消息
    };

    explicit MessagesModel(QObject *parent = nullptr);
    // QAbstractItemModel interface
    int rowCount(const QModelIndex&parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray>roleNames()const override;

    void addMessage(const FriendItem&messageItem);
    void addPreMessage(const FriendItem&messageItem);
    FriendItem getMessage(int index);

    // 在 MessagesModel 类中添加方法
    QModelIndex indexFromUid(int uid) const;

    QVector<FriendItem>getList();

private:
    QVector<FriendItem>_messages;

    // QAbstractItemModel interface
public:
    bool removeRows(int row, int count, const QModelIndex &parent)override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)override;
    bool setData(const QModelIndex &index, const QVariant &value, int role)override;
};

#endif // MESSAGESMODEL_H
