#include "frienditemdelegate.h"
#include "frienditem.h"
#include "friendsmodel.h"
#include <QFile>
#include <QPainter>
#include <QPainterPath>

FriendItemDelegate::FriendItemDelegate(QWidget* parent)
    : QStyledItemDelegate(parent)
{}

void FriendItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // 1. 绘制背景
    QRect rect = option.rect;
    int radius = 12;
    QColor bg;
    if(option.state & QStyle::State_Selected){
        bg = QColor(225,240,255);
    }else if(option.state & QStyle::State_MouseOver){
        bg = QColor(245,245,245);
    }else{
        bg = QColor("#f9fafb");
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRoundedRect(rect, radius, radius);

    // 2. 获取数据
    QString name = index.data(FriendsModel::NameRole).toString();
    QString avatarPath = index.data(FriendsModel::AvatarRole).toString();
    QString status = index.data(FriendsModel::StatusRole).toString();
    QString message = index.data(FriendsModel::MessageRole).toString();

    // 3. 绘制头像
    QRect avatarRect(rect.left() + 10, rect.top() + 10, 40, 40);
    QPixmap avatar;
    if(!avatarPath.isEmpty() && QFile::exists(avatarPath)){
        avatar = QPixmap(avatarPath).scaled(40, 40, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }else{
        avatar = QPixmap(40, 40);
        avatar.fill(QColor(200,200,200));
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
    if(status == "在线") statusColor = QColor("#58f376");
    else if(status == "忙碌") statusColor = QColor("#e90739");
    else if(status == "离开") statusColor = QColor("#51615f");
    else statusColor = QColor("#cccccc"); // 默认颜色

    QFontMetrics fm(painter->font());
    int textWidth = fm.horizontalAdvance(status);
    int dotSize = 8;  // 圆点大小
    int spacing = 5;  // 圆点和文字间距

    // 正确计算状态区域的位置
    int statusTotalWidth = dotSize + spacing + textWidth;
    int statusX = rect.right() - statusTotalWidth - 15;  // 右边距15
    int statusY = rect.top() + 15;  // 从顶部开始

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
    painter->drawText(statusTextRect, Qt::AlignLeft | Qt::AlignVCenter, status);
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

QSize FriendItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(option.rect.width(),60);
}

QPixmap FriendItemDelegate::getStatusPximap(const QString &status) const
{

}
