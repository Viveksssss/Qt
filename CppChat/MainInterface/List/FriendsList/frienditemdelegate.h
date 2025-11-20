#ifndef FRIENDITEMDELEGATE_H
#define FRIENDITEMDELEGATE_H


#include <QObject>
#include <QStyledItemDelegate>

class QListView;
class FriendsListPart;
class FriendItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FriendItemDelegate(QWidget*parent = nullptr,FriendsListPart*list=nullptr);

    // QAbstractItemDelegate interface
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QListView*getList();

private:
    QPixmap getStatusPximap(const QString &status)const;
    void showContextMenu(const QPoint &globalPos, const QModelIndex &index);
private:
    FriendsListPart*list;

    QMenu*menu;
    QAction *toTopAction;
    QAction *selectAction;
    QAction *deleteAction;

};

#endif // FRIENDITEMDELEGATE_H
