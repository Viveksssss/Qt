#include "loginscreen.h"
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QStackedWidget>
#include <QRect>
#include <qevent.h>

#include "forgotscreen.h"
#include "registerscreen.h"

LoginScreen::LoginScreen(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();

    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
    setFocusPolicy(Qt::ClickFocus);

    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.width() - width()/2,screenGeometry.height() - height()/2);
}

void LoginScreen::setupUI()
{
    this->setObjectName("loginScreen");
    this->setAttribute(Qt::WA_StyledBackground);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30,30,30,30);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignTop);

    // 头像区域
    headerLabel = new QLabel(this);
    headerLabel->setObjectName("headerLabel");
    headerLabel->setFixedSize(80,80);
    headerLabel->setAlignment(Qt::AlignHCenter);
    headerLabel->setPixmap(QPixmap(":/Resources/QQ.png"));
    headerLabel->setScaledContents(true);

    // 账号
    accountEdit = new QLineEdit(this);
    accountEdit->setObjectName("accountEdit");
    accountEdit->setFixedHeight(42);
    accountEdit->setFixedWidth(240);
    accountEdit->setAlignment(Qt::AlignHCenter);
    accountEdit->setPlaceholderText("输入QuickChat账号");
    accountEdit->clearFocus();

    // 密码
    passwordEdit = new QLineEdit(this);
    passwordEdit->setObjectName("passwordEdit");
    passwordEdit->setFixedHeight(42);
    passwordEdit->setFixedWidth(240);
    passwordEdit->setAlignment(Qt::AlignHCenter);
    passwordEdit->setPlaceholderText("输入QuickChat密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setDragEnabled(false);

    // 同意协议
    agreeCheck = new QCheckBox(this);
    agreeCheck->setObjectName("agreeCheck");
    agreeCheck->setCheckState(Qt::Unchecked);
    agreeCheck->setText("已阅读并同意服务协议和QuickChat隐私保护");
    agreeCheck->setMouseTracking(true);

    // 登陆按钮
    loginBtn = new QPushButton(this);
    loginBtn->setObjectName("loginBtn");
    loginBtn->setText("登陆");
    loginBtn->setFixedHeight(42);
    loginBtn->setFixedWidth(240);

    // 注册
    registerBtn = new QPushButton;
    registerBtn->setObjectName("registerBtn1");
    registerBtn->setText("注册账号");
    registerBtn->setCursor(Qt::PointingHandCursor);

    // 忘记密码
    forgotLabel = new QPushButton;
    forgotLabel->setObjectName("forgotLabel");
    forgotLabel->setText("忘记密码");
    forgotLabel->setCursor(Qt::PointingHandCursor);

    // 底层布局:注册,忘记密码
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setSpacing(0);
    bottomLayout->setAlignment(Qt::AlignHCenter);
    bottomLayout->addStretch(2);
    bottomLayout->addWidget(registerBtn);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(forgotLabel);
    bottomLayout->addStretch(2);

    mainLayout->addStretch();
    mainLayout->addWidget(headerLabel,0,Qt::AlignHCenter);
    mainLayout->addWidget(accountEdit,0,Qt::AlignHCenter);
    mainLayout->addWidget(passwordEdit,0,Qt::AlignHCenter);
    mainLayout->addWidget(agreeCheck,0,Qt::AlignHCenter);
    mainLayout->addWidget(loginBtn,0,Qt::AlignHCenter);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomLayout);
}


void LoginScreen::setupConnections()
{
    connect(registerBtn, &QPushButton::clicked, this, [=](){
        emit goRegsiter();
    });

    connect(forgotLabel, &QPushButton::clicked, this, [=](){
        emit goForgotPassword();
    });
}

void LoginScreen::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this); // 初始化样式选项
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this); // 使用样式绘制
    // 如果需要，你还可以在这里添加自定义绘制代码
    QWidget::paintEvent(event); // 调用基类的paintEvent
}

void LoginScreen::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    clearFocus();
}

AuthStack::AuthStack(QWidget *parent)
    :QWidget(parent)
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);

    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.width() - width()/2,screenGeometry.height() - height()/2);

    stackWidget = new QStackedWidget(this);
    loginScreen = new LoginScreen(this);
    registerScreen = new RegisterScreen(this);
    forgotScreen = new ForgotScreen(this);

    stackWidget->addWidget(loginScreen);
    stackWidget->addWidget(registerScreen);
    stackWidget->addWidget(forgotScreen);

    stackWidget->setCurrentIndex(0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(stackWidget);

    closeBtn = new QLabel(this);
    closeBtn->installEventFilter(this);
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedSize(20,20);
    closeBtn->setGeometry(sizeHint().width()-40,15,15,15);

    connect(loginScreen,&LoginScreen::goRegsiter,this,[=](){
        stackWidget->setCurrentIndex(1);
    });
    connect(loginScreen,&LoginScreen::goForgotPassword,this,[=](){
        stackWidget->setCurrentIndex(2);
    });
    connect(registerScreen,&RegisterScreen::goLogin,this,[=](){
        stackWidget->setCurrentIndex(0);
    });
    connect(forgotScreen,&ForgotScreen::goLogin,this,[=](){
        stackWidget->setCurrentIndex(0);
    });
    connect(this,&AuthStack::closeWindow,this,[=](){
        qApp->closeAllWindows();
    });
}

bool AuthStack::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == closeBtn && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            emit closeWindow();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
