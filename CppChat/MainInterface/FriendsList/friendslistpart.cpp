#include "friendslistpart.h"
#include "frienditemdelegate.h"
#include "friendsmodel.h"
#include "frienditem.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QWheelEvent>

FriendsListPart::FriendsListPart(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
    friendsModel->addFriend(FriendItem("1", "张三", ":/avatars/1.png", "在线", "今天天气真好"));
    friendsModel->addFriend(FriendItem("2", "李四", ":/avatars/2.png", "离线", "忙碌中"));
    friendsModel->addFriend(FriendItem("3", "王五", ":/avatars/3.png", "在线", "你好世界"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));
    friendsModel->addFriend(FriendItem("4", "赵六", ":/avatars/4.png", "忙碌", "请勿打扰"));

}

void FriendsListPart::setupUI()
{
    setMinimumWidth(60);
    setMaximumWidth(250);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QVBoxLayout *main_vlay = new QVBoxLayout(this);
    main_vlay->setContentsMargins(0,0,0,15);

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
    friendsDelegate = new FriendItemDelegate(this);
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
        "margin:0px;"
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

    // 添加到布局
    main_vlay->addLayout(top_hlay);
    main_vlay->addWidget(friendsList);
}

void FriendsListPart::setupConnections()
{

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
            return true; // 事件已处理
        }
    }
    return QWidget::eventFilter(obj, event);
}
