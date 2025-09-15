#ifndef SNIPATER_H
#define SNIPATER_H

#include <QObject>
#include <QSystemTrayIcon>
class Snipater : public QObject
{
    Q_OBJECT
public:
    explicit Snipater(QObject *parent = nullptr);

signals:
private:
    QSystemTrayIcon*sys;
};

#endif // SNIPATER_H
