#ifndef NOTIFICATIONPANEL_H
#define NOTIFICATIONPANEL_H

#include <QWidget>
#include <QObject>

class QListWidget;

class NotificationPanel:public QWidget
{
    Q_OBJECT
public:
    explicit NotificationPanel(QWidget*parent = nullptr);
    ~NotificationPanel() = default;
    void addFriendNews();
    void addSystemNews();
    void showPanel();
    void hidePanel();
    void setupUI();
    void setupConnections();
signals:
public slots:

private:
    QListWidget *friendsNews;
    QListWidget *systemNews;
};

#endif // NOTIFICATIONPANEL_H
