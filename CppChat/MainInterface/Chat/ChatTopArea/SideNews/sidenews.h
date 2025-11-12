#ifndef SIDENEWS_H
#define SIDENEWS_H

#include <QObject>
#include <QWidget>
// #include <../../../../Properties/global.h>

struct UserInfo;
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
    /*
     * Qt的moc无法处理模板
     *
     * 模板是编译器延迟实例化
     *
     * 而moc需要在预处理阶段工作：它无法知道你会用哪些模板参数实例化类，因此无法为所有可能的情况生成元对象代码
     *
     template<typename T>
     auto do_add_friend(T&&info,bool ok)
        -> std::enable_if_t<std::is_same_v<std::decay<T>,UserInfo>,void>{}
    */
    void do_add_friend(const UserInfo&info, bool ok); // from TcpManager::on_add_friend
    void do_auth_friend(std::shared_ptr<UserInfo>info); // from TcpManager::on_auth_friend
};


#endif // SIDENEWS_H
