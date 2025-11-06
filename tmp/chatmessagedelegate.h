// ChatMessageDelegate.h
#ifndef CHATMESSAGEDELEGATE_H
#define CHATMESSAGEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

class ChatMessageDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ChatMessageDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void messageClicked(const QString &messageId);
    void messageDoubleClicked(const QString &messageId);

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    void paintTextMessage(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void paintImageMessage(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
    void paintMixedMessage(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    QRect getMessageBubbleRect(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;
    QRect getAvatarRect(const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

    mutable QHash<QString, QPixmap> m_avatarCache;
};

#endif // CHATMESSAGEDELEGATE_H
