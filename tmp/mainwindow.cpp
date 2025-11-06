#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include "ChatMessage.h"
#include "chatwidget.h"
#include "mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    ChatWidget *chatWidget = new ChatWidget;
    layout->addWidget(chatWidget);

    chatWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    chatWidget->resize(600, 800);
    chatWidget->show();

    // 添加示例消息
    ChatMessage*msg1 = new ChatMessage;
    msg1->setId("1");
    msg1->setType(MessageType::Text);
    msg1->setRole(MessageRole::Receiver);
    msg1->setContent("你好，这是一条测试消息");
    msg1->setSenderName("对方");
    msg1->setTimestamp(QDateTime::currentDateTime());
    ChatMessage*msg3 = new ChatMessage;
    msg3->setId("1");
    msg3->setType(MessageType::Text);
    msg3->setRole(MessageRole::Receiver);
    msg3->setContent("你好，这是一条测试消息");
    msg3->setSenderName("对方");
    msg3->setTimestamp(QDateTime::currentDateTime());

    ChatMessage*msg2 = new ChatMessage;
    msg2->setId("2");
    msg2->setType(MessageType::Text);
    msg2->setRole(MessageRole::Sender);
    msg2->setContent("你好，这是我回复的消息");
    msg2->setSenderName("我");
    msg2->setTimestamp(QDateTime::currentDateTime());

    chatWidget->addMessage(*msg1);
    chatWidget->addMessage(*msg3);
    chatWidget->addMessage(*msg2);
    chatWidget->addMessage(*msg2);
    chatWidget->addMessage(*msg2);
}

MainWindow::~MainWindow()
{
    delete ui;
}
