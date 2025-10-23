#include "global.h"
#include <QDebug>
#include <QCryptographicHash>


QString gate_url_prefix = "";


std::function<QString(QString)>cryptoString = [](QString input) -> QString{
    QByteArray data = input.toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex();
};
