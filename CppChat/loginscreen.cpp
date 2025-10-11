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

#include "registerscreen.h"

LoginScreen::LoginScreen(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    // setupStyles();
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

void LoginScreen::setupStyles()
{
    setStyleSheet(R"(
        #loginScreen {
            border-image: url(:/Resources/loginBackground.png) 0 0 0 0 stretch stretch;
        }

        #headerLabel {
            border: 2px solid #E0E0E0;
            border-radius: 40px;
            background: #F8F8F8;
        }

        #accountEdit, #passwordEdit {
            border: 0px;
            border-radius: 8px;
        }

        #agreeCheck {
            font-size: 10px;
            color: #7e7f81;
        }

        #agreeCheck::indicator {
            width: 16px;
            height: 16px;
        }

        #agreeCheck::indicator:unchecked {
            image: url(:/Resources/unchecked.png);
        }

        #agreeCheck::indicator:checked {
            image: url(:/Resources/checked.png);
        }

        #agreeCheck::indicator:unchecked:hover {
            image: url(:/Resources/unchecked-hover.png);
        }

        #agreeCheck::indicator:checked:hover {
            image: url(:/Resources/checked-hover.png);
        }

        #loginBtn {
            text-align: center;
            border-radius: 8px;
            background-color: #129dd9;
            color: #f9faf0;
            font-size: 14px;
        }

        #loginBtn:hover {
            text-align: center;
            border-radius: 8px;
            background-color: #48b2e5;
            color: #cefdec;
            font-size: 14px;
        }

        #loginBtn:pressed {
            text-align: center;
            border-radius: 8px;
            background-color: #034ec4;
            color: #cefdec;
            font-size: 14px;
        }

        #registerBtn {
            background-color: transparent;
            border: none;
            color: #12a1d9;
            font-weight: bold;
        }

        #registerBtn:hover {
            background-color: transparent;
            border: none;
            color: #d3509b;
            font-weight: bold;
        }

        #forgotLabel {
            background-color: transparent;
            border: none;
            color: #12a1d9;
            font-weight: bold;
        }

        #forgotLabel:hover {
            background-color: transparent;
            border: none;
            color: #d91288;
            font-weight: bold;
        }
    )");
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

    stackWidget->addWidget(loginScreen);
    stackWidget->addWidget(registerScreen);

    stackWidget->setCurrentIndex(0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(stackWidget);

    connect(loginScreen,&LoginScreen::goRegsiter,this,[=](){
        stackWidget->setCurrentIndex(1);
    });
    connect(registerScreen,&RegisterScreen::goLogin,this,[=](){
        stackWidget->setCurrentIndex(0);
    });
}
