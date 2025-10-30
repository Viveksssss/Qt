#ifndef FRIENDSLISTPART_H
#define FRIENDSLISTPART_H


/******************************************************************************
 *
 * @file       friendslistpart.h
 * @brief      好友列表
 *
 * @author     Vivek
 * @date       2025/10/30
 * @history
 *****************************************************************************/

#include <QWidget>

class FriendsModel;
class FriendItemDelegate;
class QListView;
class QPushButton;
class QLabel;
class FriendsListPart : public QWidget
{
    Q_OBJECT
public:
    explicit FriendsListPart(QWidget *parent = nullptr);
private:
    void setupUI();
    void setupConnections();

private:
    QLabel *title;
    QPushButton *findButton;
    QListView *friendsList;
    FriendsModel *friendsModel;
    FriendItemDelegate *friendsDelegate;


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // FRIENDSLISTPART_H
