#ifndef FRIENDITEMDELEGATE_H
#define FRIENDITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class FriendItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FriendItemDelegate(QWidget*parent = nullptr);

    // QAbstractItemDelegate interface
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QPixmap getStatusPximap(const QString &status)const;
};

#endif // FRIENDITEMDELEGATE_H
