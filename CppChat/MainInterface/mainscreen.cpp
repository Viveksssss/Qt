#include "mainscreen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>
#include "FriendsList/friendslistpart.h"
#include "sidebarpart.h"
#include "toptitlepart.h"
#include "Chat/chatpart.h"

MainScreen::MainScreen(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();

    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
    setFocusPolicy(Qt::ClickFocus);
}

void MainScreen::setupUI()
{

    // 整体的layout
    QHBoxLayout *main_hlay = new QHBoxLayout;
    main_hlay->setContentsMargins(0,0,0,0);
    main_hlay->setSpacing(5);
    setLayout(main_hlay);

    QVBoxLayout *left_vlay = new QVBoxLayout;
    left_vlay->setContentsMargins(0,10,0,0);
    left_vlay->setSpacing(0);

    // toptitle
    topTitlePart = new TopTitlePart;
    left_vlay->addWidget(topTitlePart,1);

    QHBoxLayout *bar_friends_hlay = new QHBoxLayout;
    bar_friends_hlay->setContentsMargins(0,0,0,0);
    bar_friends_hlay->setSpacing(0);
    left_vlay->addLayout(bar_friends_hlay);
    // SideBarPart
    sideBarPart = new SideBarPart;
    // FriendsListPart
    friendsListPart = new FriendsListPart;

    bar_friends_hlay->addWidget(sideBarPart,0);
    bar_friends_hlay->addWidget(friendsListPart,0);

    // ChatPart
    chatPart = new ChatPart;

    main_hlay->addLayout(left_vlay,1);
    main_hlay->addWidget(chatPart,1);

    // 分割线1，sidebar和friendslist
    QFrame *sep1 = new QFrame;
    sep1->setFrameShape(QFrame::VLine);
    sep1->setFrameShadow(QFrame::Plain);
    sep1->setMaximumWidth(1);
    sep1->setStyleSheet("color: #c0c0c0;"); // 单灰线，想再淡就 #d0d0d0
    sep1->setContentsMargins(0,20,0,20);
    bar_friends_hlay->insertWidget(1,sep1);

    // 分割线2，friends和chat
    QFrame *sep2 = new QFrame;
    sep2->setFrameShape(QFrame::VLine);
    sep2->setMaximumWidth(1);
    sep2->setStyleSheet("color: #c0c0c0;");
    sep2->setContentsMargins(0,20,0,20);
    bar_friends_hlay->addWidget(sep2);

}

void MainScreen::setupConnections()
{

}
