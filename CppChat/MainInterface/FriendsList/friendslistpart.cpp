#include "friendslistpart.h"
#include "frienditemdelegate.h"
#include "friendsmodel.h"
#include "frienditem.h"
#include "../../../../Properties/global.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QWheelEvent>
#include <QMovie>
#include <QTimer>

FriendsListPart::FriendsListPart(QWidget *parent)
    : QWidget{parent}
    , isLoading{false}
{
    setupUI();
    setupConnections();

    do_loading_users();

}

QListView *FriendsListPart::getList()
{
    return friendsList;
}

void FriendsListPart::setupUI()
{
    setMinimumWidth(40);
    setMaximumWidth(220);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QVBoxLayout *main_vlay = new QVBoxLayout(this);
    main_vlay->setContentsMargins(3,0,8,15);

    QHBoxLayout *top_hlay = new QHBoxLayout;
    top_hlay->setContentsMargins(15,0,15,0);

    // title
    title = new QLabel;
    title->setText("Message");
    title->setAlignment(Qt::AlignVCenter);

    auto font = title->font();
    font.setWeight(QFont::Black);
    title->setFont(font);
    top_hlay->addWidget(title);
    top_hlay->addStretch();

    // button
    findButton = new QPushButton;
    findButton->setFixedSize({30,30});
    findButton->setObjectName("findButton");
    findButton->setIcon(QIcon(":/Resources/main/find.png"));
    findButton->setIconSize(QSize(20,20));
    top_hlay->addWidget(findButton,Qt::AlignVCenter);

    //listView
    friendsList = new QListView;
    friendsList->setObjectName("friendsList");
    friendsModel = new FriendsModel(this);
    friendsDelegate = new FriendItemDelegate(this,this);
    QScrollBar *vScrollBar = friendsList->verticalScrollBar();
    vScrollBar->setSingleStep(10);  // 每次滚轮滚动10像素
    friendsList->viewport()->installEventFilter(this);
    vScrollBar->setStyleSheet(
        "QScrollBar{"
        "background:transparent;"
        "width:3px;"
        "border-radius:3px;"
        "}"
        "QScrollBar::handle:vertical {"
        "background:#eae6e9;"
        "border-radius:10px;"
        "margin-left:2px;"
        "}"
        "QScrollBar::handle:vertical:hover{"
        "background:#efa3e2;"
        "border-radius:10px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "border: none;"
        "background: none;"
        "height: 0px;"
        "}"
    );

    friendsList->setModel(friendsModel);
    friendsList->setItemDelegate(friendsDelegate);

    friendsList->setSelectionMode(QAbstractItemView::SingleSelection);
    friendsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    friendsList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    friendsList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);   // 需要时显示
    friendsList->setMaximumWidth(250);

    // 添加到布局
    main_vlay->addLayout(top_hlay);
    main_vlay->addWidget(friendsList);
}

void FriendsListPart::setupConnections()
{
    // 滚动接受新的列表
    connect(this,&FriendsListPart::on_loading_users,this,&FriendsListPart::do_loading_users);

}

bool FriendsListPart::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == friendsList->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        QScrollBar *vScrollBar = friendsList->verticalScrollBar();
        if (vScrollBar) {
            // 自定义滚动步长
            int delta = wheelEvent->angleDelta().y();
            int step = delta > 0 ? -30 : 30;  // 反向，因为滚动条值增加是向下
            vScrollBar->setValue(vScrollBar->value() + step);

            int maxValue = vScrollBar->maximum();
            int currentValue = vScrollBar->value();
            if (currentValue - maxValue >= 0){
                qDebug() << "load more users";
                emit on_loading_users();
            }

            return true; // 事件已处理
        }
    }
    return QWidget::eventFilter(obj, event);
}


void FriendsListPart::do_loading_users()
{
    if(isLoading){
        return;
    }

    isLoading = true;

    //TODO:
    // 动态获取信息

    QTimer::singleShot(1000,this,[this](){
        this->setLoading(false);
    });
}

void FriendsListPart::do_add_friend_to_list(std::shared_ptr<UserInfo>info)
{
    friendsModel->addFriend(FriendItem(info->id, info->status,info->sex,info->name,info->avatar,info->back ));
}
