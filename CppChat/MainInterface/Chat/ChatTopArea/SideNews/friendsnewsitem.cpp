#include "friendsnewsitem.h"
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../../../../usermanager.h"
#include "../../../../Properties/sourcemanager.h"
#include "../../../../tcpmanager.h"


FriendsNewsItem::FriendsNewsItem(bool isReply,int uid,int sex,const QString &iconPath, const QString &name, const QString &content, QWidget *parent)
    : QWidget(parent)
    , _uid(uid)
    , _isRely(isReply)

{
    setupUI();
    setConnections();

    nameLabel->setText(name);
    contentLabel->setText(content);
    iconLabel->setPixmap(SourceManager::GetInstance()->getPixmap(iconPath).scaled(40,40));
}

void FriendsNewsItem::setupUI()
{
    setFixedSize({200,100});

    QVBoxLayout*main_vlay = new QVBoxLayout(this);
    main_vlay->setContentsMargins(0,10,0,10);
    main_vlay->setSpacing(0);

    iconLabel = new QLabel;
    iconLabel->setFixedSize(40,40);
    iconLabel->setObjectName("FriendsNewItem_IconLabel");

    iconLabel->setAlignment(Qt::AlignCenter);

    // 名称+内容
    QVBoxLayout*text_vlay = new QVBoxLayout;
    text_vlay->setContentsMargins(0,0,0,0);

    nameLabel = new QLabel;
    nameLabel->setObjectName("FriendsNewItem_NameLabel");

    contentLabel = new QLabel;
    contentLabel->setObjectName("FriendsNewsItem_ContentLabel");
    text_vlay->addWidget(nameLabel);
    text_vlay->addWidget(contentLabel);

    QHBoxLayout*top_hlay = new QHBoxLayout;
    top_hlay->setContentsMargins(0,0,0,0);
    top_hlay->addWidget(iconLabel);
    top_hlay->addLayout(text_vlay);


    QHBoxLayout*button_hlay = new QHBoxLayout;
    button_hlay->setContentsMargins(0,0,0,0);
    button_hlay->setSpacing(10);
    if (_isRely){
        acceptButton = new QPushButton;
        acceptButton->setObjectName("FriendsNewsItem_AcceptButton");
        acceptButton->setFixedSize(90,30);
        acceptButton->setText("确认");
        button_hlay->addWidget(acceptButton,Qt::AlignCenter);
        button_hlay->addStretch();
    }else{
        acceptButton = new QPushButton;
        acceptButton->setObjectName("FriendsNewsItem_AcceptButton");
        acceptButton->setFixedSize(90,30);
        acceptButton->setText("接受");

        rejectButton = new QPushButton;
        rejectButton->setObjectName("FriendsNewsItem_RejectButton");
        rejectButton->setFixedSize(90,30);
        rejectButton->setText("拒绝");

        button_hlay->addWidget(acceptButton);
        button_hlay->addWidget(rejectButton);
    }


    main_vlay->addLayout(top_hlay);
    main_vlay->addLayout(button_hlay);
}

void FriendsNewsItem::setConnections()
{
    if (_isRely){
        connect(acceptButton,&QPushButton::clicked,this,&FriendsNewsItem::do_accept_clicked);
    }else{
        connect(acceptButton,&QPushButton::clicked,this,&FriendsNewsItem::do_accept_clicked);
        connect(rejectButton,&QPushButton::clicked,this,&FriendsNewsItem::do_reject_clcked);
    }
}

void FriendsNewsItem::do_accept_clicked()
{
    if (_isRely){
        emit on_confirm_clicked();
    }else{
        emit on_accepted_clicked(); // 提示消除item
        // 下面回复请求为接受
        QJsonObject jsonObj;
        jsonObj["from_uid"] = UserManager::GetInstance()->GetUid();
        jsonObj["to_uid"] = _uid;
        jsonObj["accept"] = true;
        QJsonDocument doc(jsonObj);
        TcpManager::GetInstance()->do_send_data(RequestType::ID_AUTH_FRIEND_REQ,doc.toJson(QJsonDocument::Compact));
    }
}

void FriendsNewsItem::do_reject_clcked()
{
    emit on_rejected_clicked();
    // 下面回复请求为拒绝
    QJsonObject jsonObj;
    jsonObj["from_uid"] = UserManager::GetInstance()->GetUid();
    jsonObj["to_uid"] = _uid;
    jsonObj["accept"] = false;
    QJsonDocument doc(jsonObj);
    TcpManager::GetInstance()->do_send_data(RequestType::ID_AUTH_FRIEND_REQ,doc.toJson(QJsonDocument::Compact));
}
