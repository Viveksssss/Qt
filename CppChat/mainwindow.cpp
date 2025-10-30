#include "mainwindow.h"
#include "tcpmanager.h"
#include "LoginInterface/loginscreen.h"
#include "MainInterface/mainscreen.h"

#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setupUI();
    setConnections();
    stack = new AuthStack(this);
    setCentralWidget(stack);
    emit TcpManager::GetInstance()->on_switch_interface();
}

void MainWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setFixedSize({300,400});
    setWindowIcon(QIcon(":/icons/Resources/aaaaaa.png"));
    setWindowTitle("QuickChat");
    setObjectName("mainWindow");
}

void MainWindow::setConnections()
{
    // 登陆界面跳转主页面
    connect(TcpManager::GetInstance().get(),&TcpManager::on_switch_interface,this,[this](){
        mainScreen = new MainScreen;
        setCentralWidget(mainScreen);
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        // 设置窗口大小为屏幕的 80%（留出边距）
        int width = screenGeometry.width() * 0.4;
        int height = screenGeometry.height() * 0.4;

        setMinimumSize(500,300);
        setMaximumSize(1920,1080);
        // resize(width, height);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // 或者如果希望固定大小：setFixedSize(width, height);

        // 居中显示
        move(screenGeometry.width()/2 - width/2,
             screenGeometry.height()/2 - height/2);
    });
}
