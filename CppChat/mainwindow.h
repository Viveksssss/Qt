#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class AuthStack;
class MainScreen;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setupUI();
    void setConnections();

    void filesClean();
    ~MainWindow();
signals:

private:
    AuthStack *stack;
    MainScreen *mainScreen;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
