#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include "./singleton.h"

#include <QHash>
#include <QPixmap>

class SourceManager : public Singleton<SourceManager>
{
public:
    SourceManager();
    QPixmap getPixmap(const QString &path)const;

private:
    mutable QHash<QString, QPixmap> pixCache;
};

#endif // SOURCEMANAGER_H
