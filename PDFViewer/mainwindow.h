#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMainWindow>
#include <QPushButton>
#include <QTreeWidget>
#include <qfile.h>


class QPdfView;
class QPdfDocument;
class QPdfPageSelector;
class QPdfSearchModel;
class QPdfBookmarkModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void open(QUrl);
    void init();
    void openPdfFile();
private:
    void setUi();
    void addBookmark(const QModelIndex &idx, QTreeWidgetItem *parent);
signals:
public slots:
private slots:
private:
    QPdfView*viewer;
    QPdfDocument*document;
    QPdfPageSelector *pageSelector;
    QPdfSearchModel *searchModel;
    QLineEdit*lineEdit;

    QTreeWidget*treeWidget;

    QFile file;

    QDockWidget*dockWidget;
    QWidget*toolBar;

    QPushButton*openFile;
    QPushButton*zoomIn;
    QPushButton*zoomOut;
    QPushButton*reset;
    QPushButton*cursor;
    QPushButton*drag;
    QPdfBookmarkModel*model;
    bool isCursor = true;
    bool isDraging = true;
    QPoint lastDrag;

    QLabel* overlay;
    QPixmap*pixmap;

    QListView *view ;

    // QWidget interface
protected:

    bool eventFilter(QObject *watched, QEvent *event);


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
