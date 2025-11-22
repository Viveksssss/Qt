#include "mainwindow.h"
#include "tcpmanager.h"
#include "LoginInterface/loginscreen.h"
#include "MainInterface/mainscreen.h"
#include "MainInterface/Chat/ChatArea/MessageArea/messagetypes.h"
#include "usermanager.h"
#include "../database.h"

#include <QDir>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setupUI();
    setConnections();
    stack = new AuthStack(this);
    mainScreen = new MainScreen;
    setCentralWidget(stack);
    QTimer::singleShot(50,this,[this](){
        emit TcpManager::GetInstance()->on_switch_interface();
    });

    // auto&p = UserManager::GetInstance()->GetFriends();
    // auto&p2 = UserManager::GetInstance()->GetMessages();
    // auto ppp = std::make_shared<UserInfo>(1,1,1,"asdasdasd","asdasd","asdasdas");
    // ppp->desc = "哥只是个传说";
    // p.push_back(ppp);
    // auto pp = std::make_shared<ConversationItem>();
    // pp->name = "asdas";
    // pp->message = "你好啊，我是大狗熊哦嘿嘿";
    // p2.push_back(pp);

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

}

void MainWindow::setConnections()
{
    // 登陆界面跳转主页面
    connect(TcpManager::GetInstance().get(),&TcpManager::on_switch_interface,this,[this](){
        mainScreen->setParent(this);
        setCentralWidget(mainScreen);
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        // 设置窗口大小为屏幕的 80%（留出边距）
        int width = screenGeometry.width() * 0.4;
        int height = screenGeometry.height() * 0.4;
        resize(width,height);

        setMinimumSize(720,500);
        setMaximumSize(1920,1080);
        // resize(width, height);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // 或者如果希望固定大小：setFixedSize(width, height);

        // 居中显示
        move(screenGeometry.width()/2 - width/2,
             screenGeometry.height()/2 - height/2);

        // 初始化本地数据库
        DataBase::GetInstance().initialization();

        // qDebug() << DataBase::GetInstance().getFriendsPtr().size();
        // qDebug() << DataBase::GetInstance().getConversationListPtr().size();


        // UserManager::GetInstance()->SetPeerUid(1);
        UserManager::GetInstance()->GetFriends() = DataBase::GetInstance().getFriendsPtr();
        UserManager::GetInstance()->GetMessages() = DataBase::GetInstance().getConversationListPtr();

        emit TcpManager::GetInstance()->on_add_friends_to_list(UserManager::GetInstance()->GetFriendsPerPage());
        emit TcpManager::GetInstance()->on_add_messages_to_list(UserManager::GetInstance()->GetMessagesPerPage());
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
    filters << "tmp_from_quick_chat_image_*";
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

