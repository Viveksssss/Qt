#include "mainwindow.h"
#include "LoginInterface/loginscreen.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setupUI();
    AuthStack *stack = new AuthStack(this);
    setCentralWidget(stack);

}

void MainWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setFixedSize({300,400});
    setWindowIcon(QIcon(":/icons/Resources/aaaaaa.png"));
    setWindowTitle("QuickChat");
    setObjectName("mainWindow");
}
