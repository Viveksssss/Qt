#ifndef SIDENEWS_H
#define SIDENEWS_H

#include <QObject>
#include <QWidget>

class UserInfo;
class QListWidget;
class SideNews : public QWidget
{
    Q_OBJECT
public:
    explicit SideNews(QWidget *parent = nullptr);

private:
    QListWidget *friendsNewsList;
    QListWidget *systemNewsList;
public slots:
    void do_add_friend(UserInfo&&info,bool ok);
};

#endif // SIDENEWS_H
