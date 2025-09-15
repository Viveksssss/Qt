#include "snipater.h"
#include <QDebug>
#include <QString>
#include <QDir>


Snipater::Snipater(QObject *parent)
    : QObject{parent},
    sys(new QSystemTrayIcon(this))
{
    QString sysPath = QDir::currentPath()+"../../resources/icon.png";
    qDebug() << sysPath;
    this->sys->setIcon(QIcon(sysPath));
    this->sys->show();
}
