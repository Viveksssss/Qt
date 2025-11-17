#ifndef SIGNALROUTER_H
#define SIGNALROUTER_H

#include <QObject>

class SignalRouter : public QObject
{
    Q_OBJECT
public:
    explicit SignalRouter(QObject *parent = nullptr);
    static SignalRouter& GetInstance();
signals:
    void on_change_friend_status(int,int);  // from FriendsNewsItem::do_accept_clicked

public slots:

};

#endif // SIGNALROUTER_H
