#ifndef WINDOWLISTMODEL_H
#define WINDOWLISTMODEL_H

#include <QAbstractListModel>
#include <QCapturableWindow>

class WindowListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit WindowListModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QCapturableWindow window(const QModelIndex &index);
public slots:
    void populate();
private:
     QList<QCapturableWindow> windowList;
};

#endif // WINDOWLISTMODEL_H
