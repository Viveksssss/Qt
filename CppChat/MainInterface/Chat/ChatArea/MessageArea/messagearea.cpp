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

    do_load_more_message();
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
    connect(&SignalRouter::GetInstance(),&SignalRouter::on_add_new_message,this,&MessageArea::do_add_new_message);
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
    if (uid == UserManager::GetInstance()->GetPeerUid()){
        return;
    }

    auto&friends = UserManager::GetInstance()-> GetFriends();

    // Is Friend
    auto it = std::find_if(friends.begin(),friends.end(),[uid](std::shared_ptr<UserInfo> info){
        return info->id == uid;
    });

    model->clearMessage();

    if (it!=friends.end()){
        // Private
        // 首先点击了好友的列表，我们需要切换对方信息，同时发送请求获取和对方的聊天信息。
        UserManager::GetInstance()->SetPeerEmail((*it)->email);
        UserManager::GetInstance()->SetPeerDesc((*it)->desc);
        UserManager::GetInstance()->SetPeerSex((*it)->sex);
        UserManager::GetInstance()->SetPeerStatus((*it)->status);
        UserManager::GetInstance()->SetEnv(MessageEnv::Private);
        UserManager::GetInstance()->GetTimestamp().erase(UserManager::GetInstance()->GetPeerUid());
        UserManager::GetInstance()->SetPeerName((*it)->name);
        UserManager::GetInstance()->SetPeerUid(uid);
        UserManager::GetInstance()->SetPeerIcon((*it)->avatar);

        // UserManager::GetInstance()->SetEnv();
        const auto&historys = DataBase::GetInstance().getMessages((*it)->id,QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        do_change_chat_history(historys,true);

        emit SignalRouter::GetInstance().on_message_item(uid);

        do_area_to_bottom();
    }else{
        // Group
        // UserManager::GetInstance()->SetEnv(MessageEnv::Private);
        // UserManager::GetInstance()->GetTimestamp().erase(UserManager::GetInstance()->GetPeerUid());
        // UserManager::GetInstance()->SetPeerName((*it)->name);
        // UserManager::GetInstance()->SetPeerUid(uid);
        // UserManager::GetInstance()->SetPeerIcon((*it)->avatar);
    }


    // QJsonObject j ;
    // j["from_uid"] = UserManager::GetInstance()->GetUid();
    // j["to_uid"] = uid;
    // // j["timestamp"]
    // QJsonDocument doc(j);
    // TcpManager::GetInstance()->on_send_data(RequestType::ID_GET_MESSAGES_OF_FRIEND_REQ,doc.toJson());
}

void MessageArea::do_change_chat_history(std::vector<std::shared_ptr<MessageItem>>messages,bool _delete)
{
    if (_delete){
        model->clearMessage();
    }
    for (const auto&message:messages){
        model->addPreMessage(message);
    }
}

void MessageArea::do_change_chat_history(std::vector<MessageItem>messages,bool _delete)
{
    if (_delete){
        model->clearMessage();
    }
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


    QString timestamp =UserManager::GetInstance()->GetHistoryTimestamp(UserManager::GetInstance()->GetPeerUid()).toString("yyyy-MM-dd HH:mm:ss");
    const auto&historys = DataBase::GetInstance().getMessages(UserManager::GetInstance()->GetPeerUid(),timestamp);
    do_change_chat_history(historys,false);
    if (historys.size()>0){
        showToast("loading...");
    }


    QTimer::singleShot(1000,this,[this](){
        this->isLoading = false;
    });
}

void MessageArea::showToast(const QString& message, int duration)
{
    if (!toastLabel) {
        toastLabel = new QLabel(this);
        toastLabel->setAlignment(Qt::AlignCenter);
        toastLabel->setStyleSheet(
            "background-color: rgba(0, 0, 0, 180);"
            "color: white;"
            "padding: 8px 16px;"
            "border-radius: 4px;"
            "font-size: 14px;"
            );
        toastLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        toastLabel->hide();
    }

    toastLabel->setText(message);
    toastLabel->adjustSize();

    // 居中显示
    QPoint center = rect().center();
    toastLabel->move(center.x() - toastLabel->width()/2,
                     center.y() - toastLabel->height()/2);
    toastLabel->show();

    // 自动隐藏
    QTimer::singleShot(duration, toastLabel, &QLabel::hide);
}

void MessageArea::do_add_new_message(const MessageItem &item)
{
    model->addMessage(item);
    do_area_to_bottom();
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

            int minimum = vScrollBar->minimum();
            int currentValue = vScrollBar->value();
            if (currentValue <= minimum+10 && !UserManager::GetInstance()->IsLoadMessagesFinished(UserManager::GetInstance()->GetPeerUid())){
                qDebug() << "load more historys";
                emit on_load_more_message();
            }

            return true; // 事件已处理
        }
    }
    return QWidget::eventFilter(obj, event);
}
