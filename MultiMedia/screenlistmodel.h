#ifndef SCREENLISTMODEL_H
#define SCREENLISTMODEL_H

#include <QAbstractListModel>
#include <QScreen>

class ScreenListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ScreenListModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QScreen*screen(const QModelIndex&index);

private slots:
    void screensChanged();
};

#endif // SCREENLISTMODEL_H
