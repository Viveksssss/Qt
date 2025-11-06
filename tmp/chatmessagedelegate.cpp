// ChatMessageDelegate.cpp
#include "chatmessagedelegate.h"
#include "chatmessagemodel.h"
#include <QMouseEvent>

ChatMessageDelegate::ChatMessageDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void ChatMessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, option.palette.window());
    }

    // 获取消息数据
    MessageType type = static_cast<MessageType>(index.data(ChatMessageModel::MessageTypeRole).toInt());
    MessageRole role = static_cast<MessageRole>(index.data(ChatMessageModel::MessageRoleRole).toInt());
    bool isSelected = index.data(ChatMessageModel::IsSelectedRole).toBool();

    // 根据消息类型调用不同的绘制方法
    switch (type) {
    case MessageType::Text:
        paintTextMessage(painter, option, index);
        break;
    case MessageType::Image:
        paintImageMessage(painter, option, index);
        break;
    case MessageType::Mixed:
        paintMixedMessage(painter, option, index);
        break;
    default:
        paintTextMessage(painter, option, index);
        break;
    }

    // 如果消息被选中，绘制选中效果
    if (isSelected) {
        QRect bubbleRect = getMessageBubbleRect(option, index);
        painter->setPen(QPen(Qt::blue, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(bubbleRect.adjusted(-2, -2, 2, 2), 8, 8);
    }

    painter->restore();
}

void ChatMessageDelegate::paintTextMessage(QPainter *painter, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    MessageRole role = static_cast<MessageRole>(index.data(ChatMessageModel::MessageRoleRole).toInt());
    QString content = index.data(ChatMessageModel::ContentRole).toString();
    QString senderName = index.data(ChatMessageModel::SenderNameRole).toString();

    QRect bubbleRect = getMessageBubbleRect(option, index);
    QRect avatarRect = getAvatarRect(option, index);

    // 绘制头像
    painter->setBrush(Qt::lightGray);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(avatarRect);

    // 绘制发送者名称（群聊时显示）
    if (role == MessageRole::Receiver) {
        QRect nameRect(avatarRect.left(), avatarRect.top() - 20, avatarRect.width(), 15);
        painter->setPen(option.palette.text().color());
        painter->drawText(nameRect, Qt::AlignCenter, senderName);
    }

    // 绘制气泡
    QColor bubbleColor = (role == MessageRole::Sender) ? QColor("#95EC69") : QColor("#FFFFFF");
    painter->setBrush(bubbleColor);
    painter->setPen(Qt::black);
    painter->drawRoundedRect(bubbleRect, 8, 8);

    // 绘制消息内容
    painter->setPen(Qt::black);
    QRect textRect = bubbleRect.adjusted(8, 8, -8, -8);
    painter->drawText(textRect, Qt::TextWordWrap, content);
}

void ChatMessageDelegate::paintImageMessage(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    // 实现图片消息的绘制
    QVector<MediaItem> mediaItems = index.data(ChatMessageModel::MediaItemsRole).value<QVector<MediaItem>>();

    // 绘制逻辑类似文本消息，但内容区域显示图片
    // 这里简化为绘制一个矩形代表图片
    QRect bubbleRect = getMessageBubbleRect(option, index);
    QRect avatarRect = getAvatarRect(option, index);

    // 绘制头像
    painter->setBrush(Qt::lightGray);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(avatarRect);

    // 绘制气泡
    MessageRole role = static_cast<MessageRole>(index.data(ChatMessageModel::MessageRoleRole).toInt());
    QColor bubbleColor = (role == MessageRole::Sender) ? QColor("#95EC69") : QColor("#FFFFFF");
    painter->setBrush(bubbleColor);
    painter->setPen(Qt::black);
    painter->drawRoundedRect(bubbleRect, 8, 8);

    // 绘制图片占位符
    QRect imageRect = bubbleRect.adjusted(8, 8, -8, -8);
    painter->setBrush(Qt::darkGray);
    painter->drawRect(imageRect);
    painter->setPen(Qt::white);
    painter->drawText(imageRect, Qt::AlignCenter, "图片消息");
}

void ChatMessageDelegate::paintMixedMessage(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    // 混合消息的绘制，可以包含文本、图片等
    // 实现逻辑根据mediaItems来组合绘制
}

QRect ChatMessageDelegate::getMessageBubbleRect(const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    MessageRole role = static_cast<MessageRole>(index.data(ChatMessageModel::MessageRoleRole).toInt());
    int avatarSize = 40;
    int margin = 10;
    int bubbleWidth = 300; // 气泡最大宽度

    QRect rect = option.rect;

    if (role == MessageRole::Sender) {
        // 右侧消息
        return QRect(rect.right() - bubbleWidth - margin - avatarSize,
                     rect.top() + margin,
                     bubbleWidth,
                     rect.height() - 2 * margin);
    } else {
        // 左侧消息
        return QRect(rect.left() + margin + avatarSize,
                     rect.top() + margin,
                     bubbleWidth,
                     rect.height() - 2 * margin);
    }
}

QRect ChatMessageDelegate::getAvatarRect(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    MessageRole role = static_cast<MessageRole>(index.data(ChatMessageModel::MessageRoleRole).toInt());
    int avatarSize = 40;
    int margin = 10;

    QRect rect = option.rect;

    if (role == MessageRole::Sender) {
        // 右侧头像
        return QRect(rect.right() - avatarSize - margin,
                     rect.top() + margin,
                     avatarSize,
                     avatarSize);
    } else {
        // 左侧头像
        return QRect(rect.left() + margin,
                     rect.top() + margin,
                     avatarSize,
                     avatarSize);
    }
}

QSize ChatMessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 根据消息内容计算合适的高度
    MessageType type = static_cast<MessageType>(index.data(ChatMessageModel::MessageTypeRole).toInt());
    int baseHeight = 80; // 基础高度

    switch (type) {
    case MessageType::Text:
        // 文本消息根据文字长度计算高度
        return QSize(option.rect.width(), baseHeight);
    case MessageType::Image:
        return QSize(option.rect.width(), 200); // 图片消息固定高度
    default:
        return QSize(option.rect.width(), baseHeight);
    }
}

bool ChatMessageDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QString messageId = index.data(ChatMessageModel::MessageIdRole).toString();
            emit messageClicked(messageId);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QString messageId = index.data(ChatMessageModel::MessageIdRole).toString();
            emit messageDoubleClicked(messageId);
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
