#ifndef SIGNALROUTER_H
#define SIGNALROUTER_H

#include <QObject>
#include "../MainInterface/List/FriendsList/frienditem.h"

class SignalRouter : public QObject
{
    Q_OBJECT
public:
    explicit SignalRouter(QObject *parent = nullptr);
    static SignalRouter& GetInstance();
signals:
    void on_change_friend_status(int,int);          // from FriendsNewsItem::do_accept_clicked
    void on_change_friend_selection(FriendItem);    // from FriendsListPart::QListView::clicked
    void on_change_message_selection(FriendItem);   // from MessagesListPart::QListView::clicked
    void on_add_message_to_list(FriendItem);        // from FriendsItem::clicked
    void on_change_list(int);                       // from SideBar::ButtonGroup::clicked
public slots:

};

#endif // SIGNALROUTER_H
