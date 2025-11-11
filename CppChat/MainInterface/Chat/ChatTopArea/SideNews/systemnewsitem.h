#ifndef SYSTEMNEWSITEM_H
#define SYSTEMNEWSITEM_H

#include <QObject>
#include <QWidget>

class SystemNewsItem : public QWidget
{
    Q_OBJECT
public:
    explicit SystemNewsItem(QWidget *parent = nullptr);

signals:
};

#endif // SYSTEMNEWSITEM_H
