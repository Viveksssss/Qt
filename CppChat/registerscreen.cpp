#include "registerscreen.h"
#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

RegisterScreen::RegisterScreen(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    // setupStyles();
    setupConnections();

    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
    setFocusPolicy(Qt::ClickFocus);   // 允许鼠标点击时自己拿焦点

    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.width() - width()/2,screenGeometry.height() - height()/2);
}

void RegisterScreen::setupUI()
{
    this->setObjectName("registerScreen");
    this->setAttribute(Qt::WA_StyledBackground);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30,30,30,30);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignTop);

    // 表单布局
    QFormLayout *formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignHCenter);

    // 头像
    headerLabel = new QLabel;
    headerLabel->setObjectName("headerLabel");
    headerLabel->setFixedSize(80,80);
    headerLabel->setAlignment(Qt::AlignHCenter);
    headerLabel->setPixmap(QPixmap(":/Resources/QQ.png"));
    headerLabel->setScaledContents(true);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addStretch();
    headerLayout->addWidget(headerLabel);
    headerLayout->addStretch();

    // 账号
    accountEdit = new QLineEdit();
    accountEdit->setObjectName("accountEdit");
    accountEdit->setFixedHeight(30);
    accountEdit->setAlignment(Qt::AlignHCenter);
    accountEdit->setPlaceholderText("输入用户名");
    accountEdit->clearFocus();
    formLayout->addRow("账号:",accountEdit);

    // 邮箱
    mailEdit = new QLineEdit();
    mailEdit->setObjectName("mailEdit");
    mailEdit->setFixedHeight(30);
    mailEdit->setAlignment(Qt::AlignHCenter);
    mailEdit->setPlaceholderText("请输入邮箱");
    mailEdit->clearFocus();
    formLayout->addRow("邮箱:",mailEdit);

    // 密码
    passwordEdit = new QLineEdit();
    passwordEdit->setObjectName("passwordEdit");
    passwordEdit->setFixedHeight(30);
    passwordEdit->setAlignment(Qt::AlignHCenter);
    passwordEdit->setPlaceholderText("输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setDragEnabled(false);
    formLayout->addRow("密码:",passwordEdit);

    // 确认密码
    passwordSure = new QLineEdit();
    passwordSure->setObjectName("passwordSure");
    passwordSure->setFixedHeight(30);
    passwordSure->setAlignment(Qt::AlignHCenter);
    passwordSure->setPlaceholderText("确认输入QuickChat密码");
    passwordSure->setEchoMode(QLineEdit::Password);
    passwordSure->setDragEnabled(false);
    formLayout->addRow("确认密码:",passwordSure);

    // 验证码
    securityCode = new QLineEdit();
    securityCode->setObjectName("securityCode");
    securityCode->setFixedHeight(30);
    securityCode->setAlignment(Qt::AlignHCenter);
    securityCode->setPlaceholderText("输入验证码");

    getSecurityCode = new QPushButton();
    getSecurityCode->setObjectName("getSecurityCode");
    getSecurityCode->setText("获取");

    QHBoxLayout *securityCodeLayout = new QHBoxLayout;
    securityCodeLayout->addWidget(securityCode);
    securityCodeLayout->addWidget(getSecurityCode);
    formLayout->addRow("获取验证码:",securityCodeLayout);

    // 注册,取消
    registerBtn = new QPushButton();
    registerBtn->setObjectName("registerBtn2");
    registerBtn->setText("注册");
    registerBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    registerBtn->setMinimumWidth(60);
    registerBtn->setMinimumHeight(30);

    cancelBtn = new QPushButton();
    cancelBtn->setObjectName("cancelBtn");
    cancelBtn->setText("取消");
    cancelBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    cancelBtn->setMinimumWidth(60);
    cancelBtn->setMinimumHeight(30);

    QHBoxLayout *registerAndCancelLayout = new QHBoxLayout;
    registerAndCancelLayout->addWidget(registerBtn);
    registerAndCancelLayout->addWidget(cancelBtn);

    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(registerAndCancelLayout);
}

void RegisterScreen::setupStyles()
{
    setStyleSheet(R"(
        #registerScreen {
            border-image: url(:/Resources/loginBackground.png) 0 0 0 0 stretch stretch;
        }

        #headerLabel {
            border: 2px solid #E0E0E0;
            border-radius: 40px;
            background: #F8F8F8;
        }

        #accountEdit, #mailEdit, #passwordEdit, #passwordSure, #securityCode {
            border: 0px;
            border-radius: 8px;
        }

        #getSecurityCode {
            text-align: center;
            width: 50px;
            height: 27px;
            border-radius: 5px;
            background-color: #c5ddd7;
            color: #f9faf0;
            font-size: 14px;
        }

        #getSecurityCode:hover {
            text-align: center;
            border-radius: 5px;
            background-color: #bdc284;
            color: #cefdec;
            font-size: 14px;
        }

        #getSecurityCode:pressed {
            text-align: center;
            border-radius: 5px;
            background-color: #ebeec9;
            color: #cefdec;
            font-size: 14px;
        }

        #registerBtn, #cancelBtn {
            text-align: center;
            border-radius: 5px;
            background-color: #129dd9;
            color: #f9faf0;
            font-size: 15px;
        }

        #registerBtn:hover, #cancelBtn:hover {
            text-align: center;
            border-radius: 5px;
            background-color: #48b2e5;
            color: #cefdec;
            font-size: 15px;
        }

        #registerBtn:pressed, #cancelBtn:pressed {
            text-align: center;
            border-radius: 5px;
            background-color: #034ec4;
            color: #cefdec;
            font-size: 15px;
        }
    )");
}

void RegisterScreen::setupConnections()
{
    connect(cancelBtn,&QPushButton::clicked,this,[=](){
        emit goLogin();
    });
}
