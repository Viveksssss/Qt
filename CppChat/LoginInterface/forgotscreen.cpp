#include "forgotscreen.h"
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
ForgotScreen::ForgotScreen(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();

    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
    setFocusPolicy(Qt::ClickFocus);   // 允许鼠标点击时自己拿焦点

    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.width() - width()/2,screenGeometry.height() - height()/2);
}

void ForgotScreen::setupUI()
{
    this->setObjectName("ForgotScreen");
    this->setAttribute(Qt::WA_StyledBackground);
    QPushButton*cancel = new QPushButton(this);
    cancel->setText("取消");
    connect(cancel,&QPushButton::clicked,this,[=](){
        emit goLogin();
    });
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30,30,30,30);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignTop);
}

void ForgotScreen::setupConnections()
{

}
