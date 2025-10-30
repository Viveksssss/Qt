#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <QObject>
#include <QWidget>

class ChatPart;
class FriendsListPart;
class SideBarPart;
class TopTitlePart;
class MainScreen : public QWidget
{
    Q_OBJECT
public:
    explicit MainScreen(QWidget *parent = nullptr);
private:
    // 设置ui
    void setupUI();
    // 设置信号与槽
    void setupConnections();
signals:


private:
    // 聊天区域
    ChatPart *chatPart;
    // 好友列表
    FriendsListPart *friendsListPart;
    // 左侧边栏
    SideBarPart *sideBarPart;
    // 顶部边栏
    TopTitlePart *topTitlePart;
};

#endif // MAINSCREEN_H
