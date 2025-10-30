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
signals:

private:
    AuthStack *stack;
    MainScreen *mainScreen;
};

#endif // MAINWINDOW_H
