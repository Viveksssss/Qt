#include "mainwindow.h"
#include "tcpmanager.h"
#include "LoginInterface/loginscreen.h"
#include "MainInterface/mainscreen.h"
#include "usermanager.h"

#include <QDir>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setupUI();
    setConnections();
    stack = new AuthStack(this);
    setCentralWidget(stack);
    // emit TcpManager::GetInstance()->on_switch_interface();
}

void MainWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::ClickFocus);   // 允许鼠标点击时自己拿焦点
    setFixedSize({300,400});
    setWindowIcon(QIcon(":/icons/Resources/aaaaaa.png"));
    setWindowTitle("QuickChat");
    setObjectName("mainWindow");

    UserManager::GetInstance()->SetName("大胖熊");
    UserManager::GetInstance()->SetPeerName("小胖熊");
    UserManager::GetInstance()->SetAvatar(QPixmap(":/Resources/main/header-0.png"));
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

        setMinimumSize(720,500);
        setMaximumSize(1920,1080);
        // resize(width, height);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // 或者如果希望固定大小：setFixedSize(width, height);

        // 居中显示
        move(screenGeometry.width()/2 - width/2,
             screenGeometry.height()/2 - height/2);
    });
}

void MainWindow::filesClean()
{
    QDir tempDir(QDir::tempPath());
    QStringList filters;
    filters << "tmp_from_quick_chat_rounded_*";
    filters << "tmp_from_quick_chat_clipboard_*";
    filters << "tmp_from_quick_chat_aduio_*";
    filters << "tmp_from_quick_chat_video_*";
    filters << "audio_*";
    filters << "video_*";
    QFileInfoList files = tempDir.entryInfoList(filters, QDir::Files);

    for (const QFileInfo &file : files) {
        QFile::remove(file.absoluteFilePath());
    }
}

MainWindow::~MainWindow()
{
    filesClean();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制圆角矩形背景
    QPainterPath path;
    path.addRoundedRect(rect(), 12, 12);

    painter.setClipPath(path);
    painter.fillRect(rect(), Qt::white);

    // 重要：调用基类的paintEvent
    QWidget::paintEvent(event);
}

