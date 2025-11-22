#include "messagearea.h"
#include <QListView>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QJsonObject>
#include <QEvent>
#include <QWheelEvent>
#include <QTimer>
#include "./messagemodel.h"
#include "./messagedelegate.h"
#include "../../../../Properties/signalrouter.h"
#include "../../../../tcpmanager.h"
#include "../../../../database.h"

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

    list->viewport()->installEventFilter(this);
}

void MessageArea::setupConnections()
{
    connect(&SignalRouter::GetInstance(),&SignalRouter::on_change_peer,this,&MessageArea::do_change_peer);
    connect(this,&MessageArea::on_load_more_message,this,&MessageArea::do_load_more_message);
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
    qDebug() << friends.size();
    qDebug() << "uid:"<<uid;
    for (auto&p:friends){
        qDebug() << "to_uid:" << p->id;
    }
    auto it = std::find_if(friends.begin(),friends.end(),[uid](std::shared_ptr<UserInfo> info){
        return info->id == uid;
    });

    if (it!=friends.end()){
        UserManager::GetInstance()->GetTimestamp().erase(UserManager::GetInstance()->GetPeerUid());
        // 首先点击了好友的列表，我们需要切换对方信息，同时发送请求获取和对方的聊天信息。
        UserManager::GetInstance()->SetPeerName((*it)->name);
        UserManager::GetInstance()->SetPeerUid((*it)->id);
        UserManager::GetInstance()->SetPeerEmail((*it)->email);
        UserManager::GetInstance()->SetPeerDesc((*it)->desc);
        UserManager::GetInstance()->SetPeerSex((*it)->sex);
        UserManager::GetInstance()->SetPeerStatus((*it)->status);
        UserManager::GetInstance()->SetPeerIcon((*it)->avatar);

        model->clearMessage();

        QString timestamp = UserManager::GetInstance()->HasHistory((*it)->id) ? UserManager::GetInstance()->GetHistoryTimestamp((*it)->id).toString():"";
        const auto&historys = DataBase::GetInstance().getMessages((*it)->id,timestamp);
        do_change_chat_history(historys);
    }else{
        qDebug() << "wrong";
    }


    // QJsonObject j ;
    // j["from_uid"] = UserManager::GetInstance()->GetUid();
    // j["to_uid"] = uid;
    // // j["timestamp"]
    // QJsonDocument doc(j);
    // TcpManager::GetInstance()->on_send_data(RequestType::ID_GET_MESSAGES_OF_FRIEND_REQ,doc.toJson());
}

void MessageArea::do_change_chat_history(std::vector<std::shared_ptr<MessageItem>>messages)
{
    for (const auto&message:messages){
        model->addPreMessage(message);
    }
}

void MessageArea::do_change_chat_history(std::vector<MessageItem>messages)
{
    for (const auto&message:messages){
        model->addPreMessage(message);
    }
}

void MessageArea::do_load_more_message()
{
    if(isLoading){
        return;
    }
    isLoading = true;


    QString timestamp = UserManager::GetInstance()->HasHistory(UserManager::GetInstance()->GetPeerUid()) ? UserManager::GetInstance()->GetHistoryTimestamp(UserManager::GetInstance()->GetPeerUid()).toString():"";
    const auto&historys = DataBase::GetInstance().getMessages(UserManager::GetInstance()->GetPeerUid(),timestamp);
    do_change_chat_history(historys);


    QTimer::singleShot(1000,this,[this](){
        this->isLoading = false;
    });
}

void MessageArea::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

bool MessageArea::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == list->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        QScrollBar *vScrollBar = list->verticalScrollBar();
        if (vScrollBar) {
            // 自定义滚动步长
            int delta = wheelEvent->angleDelta().y();
            int step = delta > 0 ? -30 : 30;  // 反向，因为滚动条值增加是向下
            vScrollBar->setValue(vScrollBar->value() + step);

            int maxValue = vScrollBar->maximum();
            int currentValue = vScrollBar->value();
            if (currentValue <= 10 && !UserManager::GetInstance()->IsLoadFriendsFinished()){
                qDebug() << "load more historys";
                emit on_load_more_message();
            }

            return true; // 事件已处理
        }
    }
    return QWidget::eventFilter(obj, event);
}
