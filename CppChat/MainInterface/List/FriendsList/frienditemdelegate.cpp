#include "frienditemdelegate.h"
#include "frienditem.h"
#include "friendslistpart.h"
#include "../../../Properties/signalrouter.h"
#include "friendsmodel.h"
#include <QFile>
#include <QListView>
#include <QMenu>
#include <QMouseEvent>
#include <QMovie>
#include <QPainter>
#include <QPainterPath>
#include <QStandardItemModel>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

FriendItemDelegate::FriendItemDelegate(QWidget* parent,FriendsListPart*list)
    : QStyledItemDelegate(parent)
    , list(list)
{
    setupConnections();

    menu = new QMenu();
    toTopAction = new QAction("置顶",menu);
    selectAction = new QAction("选择",menu);
    deleteAction = new QAction("删除",menu);

    menu->addAction(toTopAction);
    menu->addAction(selectAction);
    menu->addAction(deleteAction);
}

void FriendItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)const
{
    painter->save();

    // 1. 绘制背景
    QRect rect = option.rect;
    int radius = 12;
    QColor bg;
    QPen pen = Qt::NoPen;
    if(option.state & QStyle::State_Selected){
        bg = QColor(225,240,255);
        pen = QPen(QColor(0xc1e6d9),2);
    }else if(option.state & QStyle::State_MouseOver){
        bg = QColor(245,245,245);
    }else{
        bg = QColor(0xf9fafb);
    }


    painter->setPen(pen);
    painter->setBrush(bg);
    painter->drawRoundedRect(rect, radius, radius);

    // 2. 获取数据
    QString name = index.data(FriendsModel::NameRole).toString();
    QString avatarPath = index.data(FriendsModel::AvatarRole).toString();
    int status = index.data(FriendsModel::StatusRole).toInt();
    QString message = index.data(FriendsModel::DescRole).toString();

    // 3. 绘制头像
    QRect avatarRect(rect.left() + 10, rect.top() + 10, 40, 40);
    QPixmap avatar;
    if(!avatarPath.isEmpty() && QFile::exists(avatarPath)){
        if (avatarPath.startsWith(":/")){
            avatar = QPixmap(avatarPath).scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        }else{
            // base64
            QByteArray imageData = QByteArray::fromBase64(avatarPath.toUtf8());
            avatar.loadFromData(imageData);
        }
    }else{
        avatar = QPixmap(":/Resources/main/header-default.png").scaled(40,40);
        // avatar.fill(QColor(200,200,200));
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath mask;
    mask.addEllipse(avatarRect);
    painter->setClipPath(mask);
    painter->drawPixmap(avatarRect, avatar);
    painter->restore();

    // 4. 状态标记 - 修正后的布局计算
    QColor statusColor;
    QString statusStr;
    if(status == 1) {
        statusColor = QColor(0x58f376);
        statusStr = "在线";
    }

    else if(status == 2){
        statusColor = QColor(0xe90739);
        statusStr = "忙碌";
    }
    else if(status == 0){
        statusColor = QColor(0x51615f);
        statusStr = "离线";
    }
    else{
        statusColor = QColor(0x51615f); // 默认颜色
        statusStr = "离线";
    }

    QFontMetrics fm(painter->font());
    int textWidth = fm.horizontalAdvance(statusStr);
    int dotSize = 8;  // 圆点大小
    int spacing = 5;  // 圆点和文字间距

    // 正确计算状态区域的位置
    int statusTotalWidth = dotSize + spacing + textWidth;
    int statusX = rect.right() - statusTotalWidth - 15;  // 右边距15
    int statusY = rect.top() + 27;  // 从顶部开始

    // 绘制状态圆点
    QRect dotRect(statusX, statusY, dotSize, dotSize);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(statusColor);
    painter->drawEllipse(dotRect);
    painter->restore();

    // 绘制状态文字
    painter->save();
    painter->setPen(statusColor);
    QFont statusFont = painter->font();
    statusFont.setPointSize(8);
    painter->setFont(statusFont);
    QRect statusTextRect(dotRect.right() + spacing, statusY - dotSize/2, textWidth, dotSize*2);
    painter->drawText(statusTextRect, Qt::AlignLeft | Qt::AlignVCenter, statusStr);
    painter->restore();

    // 5. 绘制昵称 - 修正宽度计算
    painter->save();
    painter->setPen(Qt::black);
    QFont nameFont = painter->font();
    nameFont.setPointSize(11);
    nameFont.setBold(true);
    painter->setFont(nameFont);

    // 昵称区域的宽度要避开状态区域
    int nameMaxWidth = statusX - avatarRect.right() - 20; // 减去间距
    QRect nameRect(avatarRect.right() + 10, rect.top() + 12, nameMaxWidth, 20);
    QString elidedName = painter->fontMetrics().elidedText(name, Qt::ElideRight, nameRect.width());
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, elidedName);
    painter->restore();

    // 6. 绘制消息
    painter->save();
    painter->setPen(QColor(128,128,128));
    QFont msgFont = painter->font();
    msgFont.setPointSize(9);
    painter->setFont(msgFont);

    QRect msgRect(avatarRect.right() + 10, nameRect.bottom() + 2, nameMaxWidth, 16);
    QString elidedMessage = painter->fontMetrics().elidedText(message, Qt::ElideRight, msgRect.width());
    painter->drawText(msgRect, Qt::AlignLeft | Qt::AlignVCenter, elidedMessage);
    painter->restore();

    painter->restore();
}

bool FriendItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton){
            showContextMenu(mouseEvent->globalPos(), index);
            return true;
        }
    }else if(event->type() == QEvent::MouseButtonDblClick){
        QMouseEvent*mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton){
            int id = index.data(FriendsModel::FriendRole::IdRole).toInt();
            QString name = index.data(FriendsModel::FriendRole::NameRole).toString();
            QString avatar = index.data(FriendsModel::FriendRole::AvatarRole).toString();
            int status = index.data(FriendsModel::FriendRole::StatusRole).toInt();
            QString desc = index.data(FriendsModel::FriendRole::DescRole).toString();
            emit on_open_friend_info(id,name,avatar,status,desc);
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize FriendItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(option.rect.width(),60);
}

QListView *FriendItemDelegate::getList()
{
    return list->getList();
}

void FriendItemDelegate::showContextMenu(const QPoint &globalPos, const QModelIndex &index)
{
    QAction *selectedAction = menu->exec(globalPos);
    if (!selectedAction || !index.isValid())
        return;
    QAbstractItemModel *model = const_cast<QAbstractItemModel*>(index.model());
    if (!model){
        return;
    }

    if (selectedAction == toTopAction){
        int pos = index.row();
        if (pos == 0){
            return;
        }else{
            [[maybe_unused]]bool ok = model->moveRow(QModelIndex(), pos,QModelIndex(), 0);
        }
    }else if(selectedAction == selectAction){
        auto *p= list->getList();
        if(p){
            qDebug() <<"yes";
            p->setCurrentIndex(index);
        }
    }else if(selectedAction == deleteAction){
        model->removeRow(index.row(),QModelIndex());
    }
}

void FriendItemDelegate::setupConnections()
{
    connect(this,&FriendItemDelegate::on_open_friend_info,this,&FriendItemDelegate::do_open_friend_info);
}


void FriendItemDelegate::do_open_friend_info(int uid, const QString &name, const QString &avatar, int status, const QString& desc)
{
    // 创建对话框
    QDialog* infoDialog = new QDialog();
    infoDialog->setWindowFlags(Qt::FramelessWindowHint); // 无边框窗口才能显示完整阴影
    infoDialog->setAttribute(Qt::WA_TranslucentBackground); // 透明背景
    // infoDialog->setModal(true);
    QGraphicsDropShadowEffect*shadowEffect = new QGraphicsDropShadowEffect(infoDialog);
    shadowEffect->setBlurRadius(1);
    shadowEffect->setOffset(0,0);
    shadowEffect->setColor(QColor(0,0,0));
    infoDialog->setGraphicsEffect(shadowEffect);
    infoDialog->setWindowTitle("好友信息");
    infoDialog->setFixedSize(350, 250);
    infoDialog->setWindowFlags(infoDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 设置样式
    infoDialog->setStyleSheet(
        "QDialog {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "}"
        "QLabel {"
        "    color: #333333;"
        "}"
        "QLabel#nameLabel {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QLabel#uidLabel {"
        "    font-size: 12px;"
        "    color: #666666;"
        "}"
        "QLabel#descLabel {"
        "    font-size: 13px;"
        "    color: #444444;"
        "    background-color: #f5f5f5;"
        "    border-radius: 4px;"
        "    padding: 8px;"
        "}"
        "QPushButton {"
        "    background-color: #007ACC;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #005A9E;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #004578;"
        "}"
        );

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(infoDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 上部信息区域
    QHBoxLayout* infoLayout = new QHBoxLayout();

    // 头像
    QLabel* avatarLabel = new QLabel();
    avatarLabel->setFixedSize(62, 62);

    QPixmap avatarPixmap;
    if (!avatar.isEmpty() && QFile::exists(avatar)) {
        if (avatar.startsWith(":/")) {
            avatarPixmap = QPixmap(avatar);
        } else {
            // base64 或其他格式
            QByteArray imageData = QByteArray::fromBase64(avatar.toUtf8());
            avatarPixmap.loadFromData(imageData);
        }
    } else {
        avatarPixmap = QPixmap(":/Resources/main/header-default.png");
    }

    avatarPixmap = avatarPixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    avatarLabel->setPixmap(avatarPixmap);
    avatarLabel->setStyleSheet("border-radius: 30px; border: 1px solid #e0e0e0;");

    // 右侧信息
    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(10);

    // 姓名
    QLabel* nameLabel = new QLabel(name);
    nameLabel->setObjectName("nameLabel");

    // UID
    QLabel* uidLabel = new QLabel(QString("UID: %1").arg(uid));
    uidLabel->setObjectName("uidLabel");

    textLayout->addWidget(nameLabel);
    textLayout->addWidget(uidLabel);
    textLayout->addStretch();

    infoLayout->addWidget(avatarLabel);
    infoLayout->addSpacing(15);
    infoLayout->addLayout(textLayout);
    infoLayout->addStretch();

    // 个性签名
    QLabel* descLabel = new QLabel(desc.isEmpty() ? "该用户暂时没有个性签名" : desc);
    descLabel->setObjectName("descLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignTop);
    descLabel->setFixedHeight(80);
    descLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    QPushButton* messageBtn = createIconButton(":/Resources/main/text_chat.png", "发消息", 16);
    // 语音聊天按钮
    QPushButton* voiceBtn = createIconButton(":/Resources/main/audio_chat.png", "语音聊天", 16);
    // 视频聊天按钮
    QPushButton* videoBtn = createIconButton(":/Resources/main/video_chat.png", "视频聊天", 16);

    // 连接按钮信
    connect(messageBtn, &QPushButton::clicked, infoDialog, [this, uid,infoDialog]() {

        emit SignalRouter::GetInstance().on_change_peer(uid);
        infoDialog->accept();
    });

    connect(voiceBtn, &QPushButton::clicked, infoDialog, [this, uid,infoDialog]() {
        emit SignalRouter::GetInstance().on_change_peer(uid);
        infoDialog->accept();
    });

    connect(videoBtn, &QPushButton::clicked, infoDialog, [this, uid,infoDialog]() {
        emit SignalRouter::GetInstance().on_change_peer(uid);
        infoDialog->accept();
    });

    buttonLayout->addStretch();
    buttonLayout->addWidget(messageBtn);
    buttonLayout->addWidget(voiceBtn);
    buttonLayout->addWidget(videoBtn);
    buttonLayout->addStretch();

    // 添加到主布局
    mainLayout->addLayout(infoLayout);
    mainLayout->addWidget(descLabel);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    // mainLayout->addStretch();

    // 显示在父窗口中央
    if (QWidget* parent = qobject_cast<QWidget*>(this->parent())) {
        QPoint center = parent->mapToGlobal(parent->rect().center());
        infoDialog->move(center.x() - 200, center.y() - 150);
    }

    // 显示对话框（非模态）
    infoDialog->setAttribute(Qt::WA_DeleteOnClose);
    infoDialog->show();
}


// 创建图标按钮的辅助函数
QPushButton* FriendItemDelegate::createIconButton(const QString& iconPath, const QString& text, int iconSize)
{
    QPushButton* button = new QPushButton();
    button->setFixedSize(60, 40); // 固定按钮大小

    // 设置按钮样式 - 关键在这里
    button->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f8f9fa;"
        "    border-color: #007ACC;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e9ecef;"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(button);
    // layout->setContentsMargins(5, 8, 5, 5);
    layout->setSpacing(5);

    // 图标
    QLabel* iconLabel = new QLabel();
    QPixmap iconPixmap(iconPath);
    if (iconPixmap.isNull()) {
        // 如果图标不存在，使用默认图标或文字
        iconLabel->setText("💬");
        iconLabel->setStyleSheet("font-size: 20px; background: transparent;");
    } else {
        iconPixmap = iconPixmap.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(iconPixmap);
    }
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("background: transparent;"); // 确保图标标签背景透明

    // 文字
    QLabel* textLabel = new QLabel(text);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setStyleSheet("font-size: 11px; color: #333333; background: transparent;");

    layout->addWidget(iconLabel);
    layout->addWidget(textLabel);

    return button;
}
