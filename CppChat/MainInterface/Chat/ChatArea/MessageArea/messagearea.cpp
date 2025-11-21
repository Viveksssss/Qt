#include "messagearea.h"
#include <QListView>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QJsonObject>
#include "./messagemodel.h"
#include "./messagedelegate.h"
#include "../../../../Properties/signalrouter.h"
#include "../../../../tcpmanager.h"

MessageArea::MessageArea(QWidget *parent)
    : QWidget{parent}
    , list(new QListView(this))
    , delegate(new MessageDelegate(this))
    , model(new MessageModel(this))
{
    setupUI();
    setupConnections();
}

void MessageArea::setupUI()
{
    QVBoxLayout *main_vLay = new QVBoxLayout(this);
    main_vLay->setContentsMargins(0,0,0,0);
    main_vLay->setSpacing(0);
    main_vLay->addWidget(list);

    list->setObjectName("messageList");
    list->setModel(model);
    list->setItemDelegate(delegate);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    list->setContextMenuPolicy(Qt::CustomContextMenu);
    list->setAlternatingRowColors(false);
    list->verticalScrollBar()->setSingleStep(20);
    list->setUniformItemSizes(false);   // 必须 false，才允许每行高不同
    list->setWordWrap(true);            // 内部也会触发 QTextLayout 折行
    list->setSelectionMode(QAbstractItemView::NoSelection);
    list->setFocusPolicy(Qt::ClickFocus);
}

void MessageArea::setupConnections()
{
    connect(&SignalRouter::GetInstance(),&SignalRouter::on_change_peer,this,&MessageArea::do_change_peer);
}

MessageModel *MessageArea::getModel()
{
    return this->model;
}

void MessageArea::do_area_to_bottom()
{
    list->scrollToBottom();
}

void MessageArea::do_change_peer(int uid)
{
    auto&friends = UserManager::GetInstance()-> GetFriends();
    auto it = std::find_if(friends.begin(),friends.end(),[uid](std::shared_ptr<UserInfo> info){
        return info->id == uid;
    });
    // 首先点击了好友的列表，我们需要切换对方信息，同时发送请求获取和对方的聊天信息。
    UserManager::GetInstance()->SetPeerName((*it)->name);
    UserManager::GetInstance()->SetPeerUid((*it)->id);
    UserManager::GetInstance()->SetPeerEmail((*it)->email);
    UserManager::GetInstance()->SetPeerDesc((*it)->desc);
    UserManager::GetInstance()->SetPeerSex((*it)->sex);
    UserManager::GetInstance()->SetPeerStatus((*it)->status);
    UserManager::GetInstance()->SetPeerIcon((*it)->avatar);



    QJsonObject j ;
    j["from_uid"] = UserManager::GetInstance()->GetUid();
    j["to_uid"] = uid;
    QJsonDocument doc(j);
    TcpManager::GetInstance()->on_send_data(RequestType::ID_GET_MESSAGES_OF_FRIEND_REQ,doc.toJson());
}

void MessageArea::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

}
