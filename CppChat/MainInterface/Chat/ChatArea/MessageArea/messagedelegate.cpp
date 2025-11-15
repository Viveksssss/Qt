#include "messagedelegate.h"
#include "messagemodel.h"
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTextDocument>
#include <QTextEdit>
#include <QDesktopServices>
#include <QTimer>
#include <QClipboard>
#include <QAbstractTextDocumentLayout>
#include <QMenu>
#include <QImageReader>
#include <QMessageBox>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QPushButton>
#include <QLabel>
#include <QAudioOutput>
#include <QtConcurrent/QtConcurrent>
#include <QMimeDatabase>
#include <QDir>
#include "../../../../usermanager.h"
#include "../../../../stylemanager.h"
#include "../../../../Properties/sourcemanager.h"


MessageDelegate::MessageDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
    menu = new QMenu;
    copyAction  = new QAction("复制");
    selectAction = new QAction("选择");
    deleteAction = new QAction("删除");
    menu->addAction(copyAction);
    menu->addAction(selectAction);
    menu->addAction(deleteAction);
}

void MessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    bool isMe = index.data(MessageModel::SourceRole).toInt() == static_cast<int>(MessageSource::Me);
    QColor bubbleColor = index.data(MessageModel::BubbleColorRole).value<QColor>();
    bool isSelected = index.data(MessageModel::SelectedRole).toBool();
    auto contents = index.data(MessageModel::ContentsRole).value<QList<MessageContent>>();
    QString timeText = index.data(MessageModel::DisplayTimeRole).toString();

    paintSelectionIndicator(painter,option.rect, isSelected);

    MessageType type = parseType(contents);
    switch(type){
    case MessageType::ImageMessage:
        paintPureImageMessage(painter, option, index, contents, isMe, timeText);
        break;
    case MessageType::AudioMessage:
        paintPureAudioMessage(painter, option, index, contents, isMe, timeText);
        break;
    case MessageType::VideoMessage:
        paintPureVideoMessage(painter, option, index, contents, isMe, timeText);
        break;
    case MessageType::OtherFileMessage:
        paintPureOtherFileMessage(painter, option, index, contents, isMe, timeText);
        break;
    default:
        paintBubbleMessage(painter, option, index, contents, isMe, bubbleColor,timeText);
    }
}

QSize MessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto contents = index.data(MessageModel::ContentsRole).value<QList<MessageContent>>();
    bool showAvatar = true;
    bool showUserName = true;

    QSize messageSize = calculateMessageSize(contents,option);

    // 基础高度 = 内容高度 + 气泡内边距
    int height = messageSize.height() + BUBBLE_PADDING*2;

    // 加上时间显示高度和间距
    height += 16 + 2 + AVATAR_SIZE / 2 + 2 + 8 + BUBBLE_PADDING*2;

    int width = option.rect.width();

    return QSize(width, qMax(height, 20));
}

void MessageDelegate::paintAvatar(QPainter *painter, const QRect &rect, const QPixmap &avatar) const
{
    if (avatar.isNull())
        return;

    painter->save();

    // 1. 画圆形底色
    painter->setBrush(QColor(220, 220, 220));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(rect);

    // 2. 让 painter 只在圆形区域里绘制
    QPainterPath path;
    path.addEllipse(rect);
    painter->setClipPath(path);          // 关键：圆形裁剪

    // 3. 画头像，自动被裁成圆
    QPixmap scaled = avatar.scaled(rect.size(),
                                   Qt::KeepAspectRatioByExpanding,
                                   Qt::SmoothTransformation);
    // 居中
    int x = (scaled.width()  - rect.width())  / 2;
    int y = (scaled.height() - rect.height()) / 2;
    painter->drawPixmap(rect.topLeft(), scaled.copy(x, y, rect.width(), rect.height()));

    painter->restore();
    painter->setPen(QPen(QColor("#3b3b3b"), 1));
    painter->drawEllipse(rect.adjusted(1, 1, -1, -1));

    painter->restore();
}

void MessageDelegate::paintUserName(QPainter *painter, const QRect &rect, const QString &name,bool isMe) const
{
    painter->save();

    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    painter->setPen(Qt::darkGray);

    painter->drawText(rect, isMe? Qt::AlignRight:Qt::AlignLeft | Qt::AlignVCenter, name);

    painter->restore();
}

void MessageDelegate::paintButtleBackground(QPainter *painter, const QRect &rect, const QColor &color) const
{
    painter->save();

    QPainterPath path;
    path.addRoundedRect(rect, BUBBLE_RADIUS, BUBBLE_RADIUS);

    // 绘制阴影
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 20));
    painter->drawPath(path.translated(1, 1));

    // 绘制气泡
    painter->setBrush(color);
    painter->setPen(QColor(200, 200, 200));
    painter->drawPath(path);

    painter->restore();
}

int MessageDelegate::paintTextMessage(QPainter *painter, const QRect &rect, const MessageContent &content, const QStyleOptionViewItem &option) const
{

    QString text = content.data.toString();
    QFont font = option.font;
    font.setPointSize(12);
    painter->setFont(font);
    painter->setPen(Qt::black);

    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAnywhere);
    textOption.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QRect textRect = rect.adjusted(3,3,0,0);
    QFontMetrics fm(font);
    QRect boundingRect = fm.boundingRect(textRect, Qt::TextWrapAnywhere, text);

    painter->drawText(boundingRect, text, textOption);

    return boundingRect.height();
}




int MessageDelegate::paintImageMessage(QPainter *painter, const QRect &rect, const MessageContent content, const QStyleOptionViewItem &option) const
{
    QPixmap image;
    QString imagePath = content.data.toString();

    if (imagePath.startsWith("http") || imagePath.startsWith("qrc:")) {
        // TODO:
        // 网络图片或资源图片 - 实际项目中需要异步加载
        // image = QPixmap(imagePath);
        image = SourceManager::GetInstance()->getPixmap(imagePath);
    } else {
        // image = QPixmap(imagePath);
        image = SourceManager::GetInstance()->getPixmap(imagePath);
    }

    if (image.isNull()) {
        painter->drawText(rect, Qt::AlignCenter, "图片加载失败");
        return 50;
    }

    // 缩放图片以适应宽度
    QPixmap scaledImage = image.scaled(320,180,Qt::KeepAspectRatio);
    // QPixmap scaledImage = SourceManager::GetInstance()->getPixmap(imagePath+"_rounded");
    QPoint imagePos(rect.left(), rect.top());
    painter->drawPixmap(imagePos, scaledImage);

    return scaledImage.height();
}

void MessageDelegate::paintSelectionIndicator(QPainter *painter, const QRect &rect, bool selected) const
{
    if (!selected) return;
    painter->save();
    painter->setBrush(QColor(100, 100, 255, 50));
    painter->setPen(QColor(0, 0, 255, 100));
    painter->drawRoundedRect(rect.adjusted(2, 2, -2, -2), 10, 10);
    painter->restore();
}

QSize MessageDelegate::calculateMessageSize(const QList<MessageContent> &contents,const QStyleOptionViewItem &option) const
{
    int totalHeight = 0;
    int maxWidth = 0;
    int w = textAvailWidth(option, true);
    for (const auto &content : contents) {
        switch (content.type) {
        case MessageType::TextMessage:
        {
            QString text = content.data.toString();
            QFont font;
            font.setPointSize(12);
            QFontMetrics fm(font);
            w = qMin(w,700);
            QTextDocument doc;
            doc.setDefaultFont(font);
            doc.setTextWidth(w); // 设置文本宽度限制
            doc.setPlainText(text);

            QSize textSize(doc.idealWidth(), doc.size().height());
            maxWidth = qMin(qMax(textSize.width(),maxWidth),700);

            totalHeight+=textSize.height();
            break;
        }
        break;
        case MessageType::ImageMessage:
        {
            totalHeight+=180;
            maxWidth = qMax(maxWidth,320);
            break;
        }
        case MessageType::VideoMessage:
            totalHeight += 100;
            maxWidth = qMin(qMax(200,maxWidth),700);
            break;
        case MessageType::AudioMessage:
            totalHeight += 40;
            maxWidth = qMin(qMax(100,maxWidth),700);
            break;
        case MessageType::OtherFileMessage:
            totalHeight += 80;
            maxWidth = qMin(qMax(100,maxWidth),700);
            break;
        default:
            totalHeight += 50;
            maxWidth = qMax(maxWidth, 150);
            break;
        }
    }

    if (contents.size() > 1) {
        totalHeight += (contents.size() - 1) * MESSAGE_SPACING;
    }

    return QSize(maxWidth, totalHeight);
}

QSize MessageDelegate::calculateMessageSize(const MessageContent &content,const QStyleOptionViewItem &option) const
{
    int totalHeight = 0;
    int maxWidth = 0;
    switch (content.type) {
    case MessageType::TextMessage:
    {
        QString text = content.data.toString();
        QFont font;
        font.setPointSize(12);
        QFontMetrics fm(font);
        int w = textAvailWidth(option, true);          // ← 先拿到可用宽
        QTextDocument doc;
        doc.setDefaultFont(font);
        doc.setTextWidth(w); // 设置文本宽度限制
        doc.setPlainText(text);

        QSize textSize(doc.idealWidth(), doc.size().height());

        totalHeight+=textSize.height();
        maxWidth = qMin(qMax(w,maxWidth),700);
        break;
    }
    break;
    case MessageType::ImageMessage:
    {
        totalHeight += 180;
        maxWidth = qMax(maxWidth, 320);
        break;
    }
    case MessageType::VideoMessage:
        totalHeight += 100;
        maxWidth = qMax(maxWidth, 200);
        break;
    case MessageType::AudioMessage:
        totalHeight += 40;
        maxWidth = qMax(maxWidth, 150);
        break;
    case MessageType::OtherFileMessage:
        totalHeight += 80;
        maxWidth = qMax(maxWidth,100);
        break;
    default:
        totalHeight += 50;
        maxWidth = qMax(maxWidth, 150);
        break;
    }
    return QSize(maxWidth, totalHeight);
}



bool MessageDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            // 切换选择状态
            bool currentSelection = index.data(MessageModel::SelectedRole).toBool();
            bool newSelection = !currentSelection;
            bool success = model->setData(index, newSelection, MessageModel::SelectedRole);
            emit model->dataChanged(index, index,{MessageModel::SelectedRole});
            return true;
        }else if (mouseEvent->button() == Qt::RightButton) {
            // 右键点击，显示复制菜单
            showContextMenu(mouseEvent->globalPos(), index);
            return true;
        }
    }else if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        auto contents = index.data(MessageModel::ContentsRole).value<QList<MessageContent>>();

        // 检查是否为纯媒体消息
        if (contents.size() == 1) {
            MessageType type = contents.first().type;
            QString filePath = contents.first().data.toString();

            if (type == MessageType::ImageMessage || type == MessageType::VideoMessage || type == MessageType::AudioMessage) {
                // QDesktopServices::openUrl(QUrl(filePath));
                openFile(filePath);
                return true;
            }else if (type == MessageType::OtherFileMessage){
                QDesktopServices::openUrl(QUrl(filePath));
                return true;
            }
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void MessageDelegate::showContextMenu(const QPoint &globalPos, const QModelIndex &index)
{
    // 显示菜单并处理选择
    QAction *selectedAction = menu->exec(globalPos);
    if (!selectedAction || !index.isValid()){
        return;
    }

    if (selectedAction == copyAction) {

        int type = index.data(MessageModel::TypeRole).toInt();
        const auto&contents = index.data(MessageModel::ContentsRole).value<QList<MessageContent>>();
        switch(type){
        case static_cast<int>(MessageType::TextMessage):
        {
            QString messageText = contents.first().data.toString();
            if (messageText.isEmpty()) {
                return;
            }
            // 缓存消息文本
            cachedMessageText = messageText;
            QApplication::clipboard()->setText(cachedMessageText);
            break;
        }
        case static_cast<int>(MessageType::ImageMessage):
        {
            QString imagePath = contents.first().data.value<QString>();

            // 在新线程中加载图片
            QtConcurrent::run([imagePath]() {
                QPixmap img = SourceManager::GetInstance()->getPixmap(imagePath);
                if (!img.isNull()) {
                    // 在主线程中设置剪贴板
                    QMetaObject::invokeMethod(qApp, [img]() {
                        QApplication::clipboard()->setPixmap(img);
                    }, Qt::QueuedConnection);
                }
            });
            break;
        }
        case static_cast<int>(MessageType::AudioMessage):
        case static_cast<int>(MessageType::VideoMessage):
            QString str = contents.first().data.toString();
            QApplication::clipboard()->setText(str);
            break;
        dafault:
            break;
        }
    }
    else if (selectedAction == selectAction) {
        QAbstractItemModel *model = const_cast<QAbstractItemModel*>(index.model());
        if (!model)return;
        model->setData(index, true, MessageModel::SelectedRole);
    }else if(selectedAction == deleteAction){
        QMessageBox msgBox;
        msgBox.setWindowTitle("删除消息");
        msgBox.setText("确定要删除这条消息吗？");
        msgBox.setInformativeText("只会在您的记录中删除");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Question);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            QAbstractItemModel *model = const_cast<QAbstractItemModel*>(index.model());
            if (!model)return;
            bool success = model->removeRow(index.row());
            if (success) {
                qDebug() << "消息删除成功";
            } else {
                QMessageBox::warning(nullptr, "错误", "删除失败，请重试");
            }
        }
    }
}

int MessageDelegate::textAvailWidth(const QStyleOptionViewItem &option, bool showAvatar) const
{
    return option.rect.width()
    - (showAvatar ? AVATAR_SIZE + AVATAR_MARGIN*2 : 0)
        - BUBBLE_PADDING * 2
        - 50;   //
}

void MessageDelegate::paintBubbleMessage(QPainter *painter, const QStyleOptionViewItem &option,
                                         const QModelIndex &index, const QList<MessageContent> &contents,
                                         bool isMe, const QColor &bubbleColor, const QString &timeText) const
{
    bool showUserName = true;
    bool showAvatar = true;

    QSize size = calculateMessageSize(contents, option);

    QVector<QPair<MessageContent, QSize>> contentSizes;
    for (const auto& content : contents) {
        QSize contentSize = calculateMessageSize(content, option);
        contentSizes.append(qMakePair(content, contentSize));
    }

    QRect avatarRect;
    QRect bubbleRect;
    QRect userNameRect;
    int bubbleWidth = size.width() + BUBBLE_PADDING * 2;
    int bubbleHeight = size.height() + BUBBLE_PADDING * 2;

    bubbleRect.setWidth(bubbleWidth);
    bubbleRect.setHeight(bubbleHeight);

    if (isMe){
        avatarRect = QRect(option.rect.right() - AVATAR_SIZE - AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        bubbleRect = QRect(BUBBLE_PADDING + avatarRect.left() - AVATAR_MARGIN - bubbleRect.width(),
                           (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                           bubbleRect.width(), bubbleHeight);
        if(showUserName){
            userNameRect = QRect(bubbleRect.left() - 5, option.rect.top() + 8,
                                 bubbleRect.width(), USER_NAME_HEIGHT);
        }
    } else {
        avatarRect = QRect(option.rect.left() + AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        bubbleRect = QRect(BUBBLE_PADDING + option.rect.left() + 5 + AVATAR_SIZE + AVATAR_MARGIN,
                           (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                           bubbleRect.width(), bubbleHeight);
        if(showUserName){
            userNameRect = QRect(bubbleRect.left() + 5, option.rect.top() + 8,
                                 bubbleRect.width(), USER_NAME_HEIGHT);
        }
    }

    if (showAvatar){
        paintAvatar(painter, avatarRect,
                    (isMe ? UserManager::GetInstance()->GetAvatar() : UserManager::GetInstance()->GetPeerAvatar()));
    }

    if (showUserName){
        paintUserName(painter, userNameRect,
                      isMe ? UserManager::GetInstance()->GetName() : UserManager::GetInstance()->GetPeerName(),
                      isMe);
    }

    paintButtleBackground(painter, bubbleRect, bubbleColor);
    QRect contentRect = bubbleRect.adjusted(BUBBLE_PADDING, BUBBLE_PADDING, -BUBBLE_PADDING, -BUBBLE_PADDING);

    int currentY = contentRect.top();
    for (const auto& [content, size] : contentSizes){
        QRect elementRect(contentRect.left()  ,
                          currentY ,
                          contentRect.width(), size.height());
        switch (content.type) {
        case MessageType::TextMessage:
            paintTextMessage(painter, elementRect, content, option);
            break;
        case MessageType::ImageMessage:
            paintImageMessage(painter, elementRect, content, option);
            break;
        case MessageType::VideoMessage:
            elementRect.setHeight(120);
            painter->setBrush(QColor(240, 240, 240));
            painter->drawRoundedRect(elementRect, 4, 4);
            painter->drawText(elementRect, Qt::AlignCenter, "▶ 视频消息");
            break;
        case MessageType::AudioMessage:
            elementRect.setHeight(40);
            painter->setBrush(QColor(240, 240, 240));
            painter->drawRoundedRect(elementRect, 20, 20);
            painter->drawText(elementRect, Qt::AlignCenter, "🔊 音频消息");
            break;
        default:
            elementRect.setHeight(30);
            painter->drawText(elementRect, Qt::AlignCenter, "未知消息类型");
            break;
        }

        currentY += elementRect.height() + MESSAGE_SPACING;
    }

    QFont timeFont = option.font;
    timeFont.setPointSize(9);
    painter->setFont(timeFont);
    painter->setPen(Qt::gray);
    QRect timeRect(bubbleRect.left(), bubbleRect.bottom() + 2, bubbleRect.width(), 16);
    painter->drawText(timeRect, Qt::AlignCenter, timeText);
}

void MessageDelegate::paintRoundedImage(QPainter *painter, const QRect &rect, const QString &imagePath) const
{
    QPixmap image;
    if (imagePath.startsWith("http") || imagePath.startsWith("qrc:")) {
        // image = QPixmap(imagePath);
        image = SourceManager::GetInstance()->getPixmap(imagePath+"_rounded");
    } else {
        image = SourceManager::GetInstance()->getPixmap(imagePath+"_rounded");
    }

    if (image.isNull()) {
        painter->drawText(rect, Qt::AlignCenter, "图片加载失败");
        return;
    }

    // 缩放图片
    // QPixmap scaledImage = image.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // QPixmap scaledImage = SourceManager::GetInstance()->getPixmap("");

    // 绘制圆角效果
    painter->save();

    QPainterPath path;
    path.addRoundedRect(rect, 12, 12);  // 12px圆角
    painter->setClipPath(path);

    // 居中绘制图片
    QPoint drawPos(rect.left() + (rect.width() - image.width()) / 2,
                   rect.top() + (rect.height() - image.height()) / 2);
    painter->drawPixmap(drawPos, image);

    painter->restore();

    // 可选：添加轻微阴影或边框
    painter->save();
    painter->setPen(QColor(0, 0, 0, 30));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(rect.adjusted(0, 0, -1, -1), 12, 12);
    painter->restore();
}

void MessageDelegate::paintFileMessageContent(QPainter *painter, const QRect &rect, const MessageContent &content) const
{
    QString filePath = content.data.toString();
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    qint64 fileSize = fileInfo.size();
    QString fileSuffix = fileInfo.suffix().toUpper();

    painter->save();

    // 绘制文件卡片背景
    QPainterPath path;
    path.addRoundedRect(rect, 8, 8);

    // 绘制阴影
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 20));
    painter->drawPath(path.translated(1, 1));

    // 绘制背景
    painter->setBrush(QColor(240, 240, 240));
    painter->setPen(QColor(200, 200, 200));
    painter->drawPath(path);

    // 绘制文件图标区域（上半部分）
    QRect iconRect(rect.left() + 10, rect.top() + 10, rect.width() - 20, 40);
    paintFileIcon(painter, iconRect, fileSuffix);

    // 绘制文件名（下半部分）
    QFont font = painter->font();
    font.setPointSize(8);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::black);

    QRect nameRect(rect.left() + 5, rect.top() + 55, rect.width() - 10, 20);
    QString elidedName = painter->fontMetrics().elidedText(fileName, Qt::ElideRight, nameRect.width());
    painter->drawText(nameRect, Qt::AlignCenter, elidedName);

    painter->restore();
}

void MessageDelegate::paintFileIcon(QPainter *painter, const QRect &rect, const QString &fileSuffix) const
{
    painter->save();

    // 根据文件类型设置不同颜色
    QColor iconColor;
    if (fileSuffix == "PDF") {
        iconColor = QColor(255, 0, 0); // 红色
    } else if (fileSuffix == "DOC" || fileSuffix == "DOCX") {
        iconColor = QColor(0, 0, 255); // 蓝色
    } else if (fileSuffix == "XLS" || fileSuffix == "XLSX") {
        iconColor = QColor(0, 128, 0); // 绿色
    } else if (fileSuffix == "ZIP" || fileSuffix == "RAR" || fileSuffix == "7Z") {
        iconColor = QColor(255, 165, 0); // 橙色
    } else {
        iconColor = QColor(100, 100, 100); // 灰色
    }

    // 绘制文件图标（简单的文件形状）
    painter->setBrush(iconColor);
    painter->setPen(QColor(iconColor.darker()));

    // 文件主体
    QRect fileBody(rect.left(), rect.top(), rect.width() * 0.8, rect.height());
    painter->drawRoundedRect(fileBody, 4, 4);

    // 文件折叠角
    QPolygon foldCorner;
    foldCorner << QPoint(fileBody.right(), fileBody.top())
               << QPoint(fileBody.right(), fileBody.top() + rect.height() * 0.3)
               << QPoint(fileBody.right() - rect.width() * 0.2, fileBody.top())
               << QPoint(fileBody.right(), fileBody.top());
    painter->setBrush(iconColor.lighter(120));
    painter->drawPolygon(foldCorner);

    // 文件后缀文字
    painter->setPen(Qt::white);
    QFont font = painter->font();
    font.setPointSize(10);
    font.setBold(true);
    painter->setFont(font);

    QRect textRect = fileBody.adjusted(2, 2, -2, -2);
    painter->drawText(textRect, Qt::AlignCenter, fileSuffix.left(3));

    painter->restore();
}

void MessageDelegate::paintVideoMessageContent(QPainter *painter, const QRect &rect, const MessageContent &content) const
{
    QString videoPath = content.data.toString();

    // 绘制视频封面背景
    painter->save();

    // 绘制圆角矩形背景
    QPainterPath path;
    path.addRoundedRect(rect, 12, 12);
    painter->setClipPath(path);

    // 绘制渐变背景（模拟视频封面）
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(80, 80, 100));
    gradient.setColorAt(1, QColor(50, 50, 70));
    painter->fillRect(rect, gradient);

    painter->restore();

    // 绘制边框
    painter->save();
    painter->setPen(QColor(0, 0, 0, 80));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(rect, 12, 12);
    painter->restore();

    // 绘制播放按钮
    paintPlayButton(painter, rect);

    // 绘制视频文件名
    paintVideoFileName(painter, rect, videoPath);

    // TODO:绘制视频图标:暂时不绘制，以后可能会有扩展
    // paintVideoIcon(painter, rect);
}

// 绘制播放按钮
void MessageDelegate::paintPlayButton(QPainter *painter, const QRect &rect) const
{
    painter->save();

    // 绘制圆形播放按钮背景
    int buttonSize = 40;
    QRect buttonRect(rect.center().x() - buttonSize/2,
                     rect.center().y() - buttonSize/2,
                     buttonSize, buttonSize);

    // 圆形背景
    painter->setBrush(QColor(255, 255, 255, 200));
    painter->setPen(QColor(255, 255, 255, 150));
    painter->drawEllipse(buttonRect);

    // 播放三角形
    painter->setBrush(QColor(0, 0, 0, 180));
    painter->setPen(Qt::NoPen);

    QPolygon triangle;
    int triangleSize = 15;
    triangle << QPoint(rect.center().x() - triangleSize/3, rect.center().y() - triangleSize/2)
             << QPoint(rect.center().x() - triangleSize/3, rect.center().y() + triangleSize/2)
             << QPoint(rect.center().x() + triangleSize/2, rect.center().y());
    painter->drawPolygon(triangle);

    painter->restore();
}

// 绘制视频文件名
void MessageDelegate::paintVideoFileName(QPainter *painter, const QRect &rect, const QString &videoPath) const
{
    painter->save();

    QFileInfo fileInfo(videoPath);
    QString fileName = fileInfo.fileName();

    QFont font = painter->font();
    font.setPointSize(8);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(Qt::white);

    // 文件名显示在底部
    QRect textRect = rect.adjusted(8, rect.height() - 25, -8, -8);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                      painter->fontMetrics().elidedText(fileName, Qt::ElideRight, textRect.width()));

    painter->restore();
}

// 绘制视频图标
void MessageDelegate::paintVideoIcon(QPainter *painter, const QRect &rect) const
{
    painter->save();

    // 在左上角绘制视频图标
    QRect iconRect(rect.left() + 8, rect.top() + 8, 20, 20);

    // 绘制摄像机图标（简单的图形表示）
    painter->setBrush(QColor(255, 255, 255, 180));
    painter->setPen(QColor(255, 255, 255, 220));

    // 摄像机主体
    painter->drawRoundedRect(iconRect, 4, 4);

    // 镜头
    painter->setBrush(QColor(100, 100, 150, 200));
    painter->drawEllipse(iconRect.center(), 4, 4);

    painter->restore();
}




MessageType MessageDelegate::parseType(const QList<MessageContent> &contents) const
{
    if (contents.size() != 1) {
        return MessageType::MixedMessage;
    }
    return contents.first().type;
}

void MessageDelegate::paintPureOtherFileMessage(QPainter *painter, const QStyleOptionViewItem &option,
                                                const QModelIndex &index, const QList<MessageContent> &contents,
                                                bool isMe, const QString &timeText) const
{
    bool showUserName = true;
    bool showAvatar = true;

    const MessageContent &fileContent = contents.first();
    QString filePath = fileContent.data.toString();
    QFileInfo fileInfo(filePath);

    // 计算文件消息尺寸
    QSize fileSize(100,80); // 固定尺寸

    // 计算布局
    QRect avatarRect;
    QRect fileRect;
    QRect userNameRect;

    if (isMe) {
        avatarRect = QRect(option.rect.right() - AVATAR_SIZE - AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        fileRect = QRect(BUBBLE_PADDING + avatarRect.left() - AVATAR_MARGIN - fileSize.width(),
                          (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                          fileSize.width(), fileSize.height());
        if (showUserName) {
            userNameRect = QRect(fileRect.left() - 5, option.rect.top() + 8,
                                 fileRect.width(), USER_NAME_HEIGHT);
        }
    } else {
        avatarRect = QRect(option.rect.left() + AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        fileRect = QRect(BUBBLE_PADDING + option.rect.left() + 5 + AVATAR_SIZE + AVATAR_MARGIN,
                          (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                          fileSize.width(), fileSize.height());
        if (showUserName) {
            userNameRect = QRect(fileRect.left() + 5, option.rect.top() + 8,
                                 fileRect.width(), USER_NAME_HEIGHT);
        }
    }

    // 绘制头像
    if (showAvatar) {
        paintAvatar(painter, avatarRect,
                    (isMe ? UserManager::GetInstance()->GetAvatar() : UserManager::GetInstance()->GetPeerAvatar()));
    }

    // 绘制用户名
    if (showUserName) {
        paintUserName(painter, userNameRect,
                      isMe ? UserManager::GetInstance()->GetName() : UserManager::GetInstance()->GetPeerName(),
                      isMe);
    }

    // 绘制文件消息内容
    paintFileMessageContent(painter, fileRect, fileContent);

    // 绘制时间
    QFont timeFont = option.font;
    timeFont.setPointSize(9);
    painter->setFont(timeFont);
    painter->setPen(Qt::gray);
    QRect timeRect(fileRect.left(), fileRect.bottom() + 2, fileRect.width(), 16);
    painter->drawText(timeRect, Qt::AlignCenter, timeText);
}

void MessageDelegate::paintPureImageMessage(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index, const QList<MessageContent> &contents,
                                            bool isMe, const QString &timeText) const
{
    bool showUserName = true;
    bool showAvatar = true;

    const MessageContent &imageContent = contents.first();

    // 计算图片尺寸
    // QSize imageSize = calculateImageSize(imageContent.data.toString());
    // QSize scaledSize = calculatePureImageRect(option.rect, imageSize, isMe).size();
    QSize scaledSize = {320,180};

    // 计算布局（保持与气泡消息类似的布局）
    QRect avatarRect;
    QRect imageRect;
    QRect userNameRect;

    if (isMe){
        avatarRect = QRect(option.rect.right() - AVATAR_SIZE - AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        imageRect = QRect(BUBBLE_PADDING + avatarRect.left() - AVATAR_MARGIN - scaledSize.width(),
                           (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                          scaledSize.width(), scaledSize.height());
        if(showUserName){
            userNameRect = QRect(imageRect.left() - 5, option.rect.top() + 8,
                                 scaledSize.width(), USER_NAME_HEIGHT);
        }
    } else {
        avatarRect = QRect(option.rect.left() + AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        imageRect = QRect(BUBBLE_PADDING + option.rect.left() + 5 + AVATAR_SIZE + AVATAR_MARGIN,
                           (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                           scaledSize.width(), scaledSize.height());
        if(showUserName){
            userNameRect = QRect(imageRect.left() + 5, option.rect.top() + 8,
                                 scaledSize.width(), USER_NAME_HEIGHT);
        }
    }

    // 绘制头像
    if (showAvatar) {
        paintAvatar(painter, avatarRect,
                    (isMe ? UserManager::GetInstance()->GetAvatar() : UserManager::GetInstance()->GetPeerAvatar()));
    }

    // 绘制用户名
    if (showUserName) {
        paintUserName(painter, userNameRect,
                      isMe ? UserManager::GetInstance()->GetName() : UserManager::GetInstance()->GetPeerName(),
                      isMe);
    }

    // 绘制圆角图片（没有气泡背景）
    paintRoundedImage(painter, imageRect, imageContent.data.toString());

    // 绘制时间（放在图片下方居中）
    QFont timeFont = option.font;
    timeFont.setPointSize(9);
    painter->setFont(timeFont);
    painter->setPen(Qt::gray);
    QRect timeRect(imageRect.left(), imageRect.bottom() + 2, imageRect.width(), 16);
    painter->drawText(timeRect, Qt::AlignCenter, timeText);
}

void MessageDelegate::paintPureVideoMessage(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QList<MessageContent> &contents, bool isMe, const QString &timeText) const
{
    bool showUserName = true;
    bool showAvatar = true;

    const MessageContent &videoContent = contents.first();

    // 计算图片尺寸
    // QSize imageSize = calculateImageSize(imageContent.data.toString());
    // QSize scaledSize = calculatePureImageRect(option.rect, imageSize, isMe).size();
    QSize scaledSize = {200,100};

    // 计算布局（保持与气泡消息类似的布局）
    QRect avatarRect;
    QRect videoRect;
    QRect userNameRect;

    if (isMe){
        avatarRect = QRect(option.rect.right() - AVATAR_SIZE - AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        videoRect = QRect(BUBBLE_PADDING + avatarRect.left() - AVATAR_MARGIN - scaledSize.width(),
                          (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                          scaledSize.width(), scaledSize.height());
        if(showUserName){
            userNameRect = QRect(videoRect.left() - 5, option.rect.top() + 8,
                                 scaledSize.width(), USER_NAME_HEIGHT);
        }
    } else {
        avatarRect = QRect(option.rect.left() + AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        videoRect = QRect(BUBBLE_PADDING + option.rect.left() + 5 + AVATAR_SIZE + AVATAR_MARGIN,
                          (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                          scaledSize.width(), scaledSize.height());
        if(showUserName){
            userNameRect = QRect(videoRect.left() + 5, option.rect.top() + 8,
                                 scaledSize.width(), USER_NAME_HEIGHT);
        }
    }

    // 绘制头像
    if (showAvatar) {
        paintAvatar(painter, avatarRect,
                    (isMe ? UserManager::GetInstance()->GetAvatar() : UserManager::GetInstance()->GetPeerAvatar()));
    }

    // 绘制用户名
    if (showUserName) {
        paintUserName(painter, userNameRect,
                      isMe ? UserManager::GetInstance()->GetName() : UserManager::GetInstance()->GetPeerName(),
                      isMe);
    }

    // 绘制圆角图片（没有气泡背景）
    paintVideoMessageContent(painter, videoRect, videoContent);

    // 绘制时间（放在图片下方居中）
    QFont timeFont = option.font;
    timeFont.setPointSize(9);
    painter->setFont(timeFont);
    painter->setPen(Qt::gray);
    QRect timeRect(videoRect.left(), videoRect.bottom() + 2, videoRect.width(), 16);
    painter->drawText(timeRect, Qt::AlignCenter, timeText);
}

void MessageDelegate::paintPureAudioMessage(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QList<MessageContent> &contents, bool isMe, const QString &timeText) const
{
    bool showUserName = true;
    bool showAvatar = true;

    const MessageContent &audioContent = contents.first();

    // 音频消息尺寸
    QSize audioSize = {250, 60}; // 音频消息比视频窄一些，但高度适合控制按钮

    // 计算布局（保持与视频消息类似的布局）
    QRect avatarRect;
    QRect audioRect;
    QRect userNameRect;

    if (isMe){
        avatarRect = QRect(option.rect.right() - AVATAR_SIZE - AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        audioRect = QRect(BUBBLE_PADDING + avatarRect.left() - AVATAR_MARGIN - audioSize.width(),
                          (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                          audioSize.width(), audioSize.height());
        if(showUserName){
            userNameRect = QRect(audioRect.left() - 5, option.rect.top() + 8,
                                 audioSize.width(), USER_NAME_HEIGHT);
        }
    } else {
        avatarRect = QRect(option.rect.left() + AVATAR_MARGIN,
                           option.rect.top() + AVATAR_MARGIN,
                           AVATAR_SIZE, AVATAR_SIZE);
        audioRect = QRect(BUBBLE_PADDING + option.rect.left() + 5 + AVATAR_SIZE + AVATAR_MARGIN,
                          (avatarRect.bottom() + avatarRect.top()) / 2 + 2,
                          audioSize.width(), audioSize.height());
        if(showUserName){
            userNameRect = QRect(audioRect.left() + 5, option.rect.top() + 8,
                                 audioSize.width(), USER_NAME_HEIGHT);
        }
    }

    // 绘制头像
    if (showAvatar) {
        paintAvatar(painter, avatarRect,
                    (isMe ? UserManager::GetInstance()->GetAvatar() : UserManager::GetInstance()->GetPeerAvatar()));
    }

    // 绘制用户名
    if (showUserName) {
        paintUserName(painter, userNameRect,
                      isMe ? UserManager::GetInstance()->GetName() : UserManager::GetInstance()->GetPeerName(),
                      isMe);
    }

    // 绘制音频消息内容
    paintAudioMessageContent(painter, audioRect, audioContent);

    // 绘制时间（放在音频消息下方居中）
    QFont timeFont = option.font;
    timeFont.setPointSize(9);
    painter->setFont(timeFont);
    painter->setPen(Qt::gray);
    QRect timeRect(audioRect.left(), audioRect.bottom() + 2, audioRect.width(), 16);
    painter->drawText(timeRect, Qt::AlignCenter, timeText);
}


void MessageDelegate::paintAudioMessageContent(QPainter *painter, const QRect &rect, const MessageContent &content) const
{
    QString audioPath = content.data.toString();
    QFileInfo fileInfo(audioPath);
    QString fileName = fileInfo.fileName();

    // 尝试获取音频时长
    QString duration = getAudioDuration(audioPath);

    painter->save();

    // 绘制音频消息背景
    QPainterPath path;
    path.addRoundedRect(rect, 12, 12);

    // 绘制阴影
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 20));
    painter->drawPath(path.translated(1, 1));

    // 绘制音频消息背景
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(70, 130, 180));  // 钢蓝色
    gradient.setColorAt(1, QColor(65, 105, 225));  // 皇家蓝
    painter->setBrush(gradient);
    painter->setPen(QColor(100, 149, 237));
    painter->drawPath(path);

    // 绘制播放按钮
    paintAudioPlayButton(painter, rect);

    // 绘制音频信息
    paintAudioInfo(painter, rect, fileName, duration);

    // 绘制波形图
    paintAudioWaveform(painter, rect);

    painter->restore();
}

void MessageDelegate::paintAudioPlayButton(QPainter *painter, const QRect &rect) const
{
    painter->save();

    // 绘制圆形播放按钮
    int buttonSize = 36;
    QRect buttonRect(rect.left() + 15,
                     rect.top() + (rect.height() - buttonSize) / 2,
                     buttonSize, buttonSize);

    // 按钮背景
    painter->setBrush(QColor(255, 255, 255, 220));
    painter->setPen(QColor(255, 255, 255, 180));
    painter->drawEllipse(buttonRect);

    // 播放图标（三角形）
    painter->setBrush(QColor(70, 130, 180));
    painter->setPen(Qt::NoPen);

    QPolygon triangle;
    int triangleSize = 12;
    triangle << QPoint(buttonRect.center().x() - triangleSize/3 + 1, buttonRect.center().y() - triangleSize/2)
             << QPoint(buttonRect.center().x() - triangleSize/3 + 1, buttonRect.center().y() + triangleSize/2)
             << QPoint(buttonRect.center().x() + triangleSize/2 + 1, buttonRect.center().y());
    painter->drawPolygon(triangle);

    painter->restore();
}

void MessageDelegate::paintAudioInfo(QPainter *painter, const QRect &rect, const QString &fileName, const QString &duration) const
{
    painter->save();

    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);
    painter->setPen(Qt::white);

    // 绘制文件名
    QRect nameRect(rect.left() + 60, rect.top() + 8, rect.width() - 75, 20);
    QString elidedName = painter->fontMetrics().elidedText(fileName, Qt::ElideRight, nameRect.width());
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, elidedName);

    // 绘制时长
    QFont smallFont = painter->font();
    smallFont.setPointSize(9);
    painter->setFont(smallFont);
    painter->setPen(QColor(240, 240, 240));

    QRect durationRect(nameRect.left(), nameRect.bottom() + 12, nameRect.width(), 16);
    QString durationText = duration.isEmpty() ? "未知时长" : duration;
    painter->drawText(durationRect, Qt::AlignLeft | Qt::AlignVCenter, durationText);

    painter->restore();
}

void MessageDelegate::paintAudioWaveform(QPainter *painter, const QRect &rect) const
{
    painter->save();

    // 绘制简单的波形图
    int waveHeight = 8;
    int waveTop = rect.top() + (rect.height() - waveHeight) / 2 + 5;
    int waveLeft = rect.left() + 60;
    int waveWidth = rect.width() - 75;

    painter->setPen(QPen(QColor(255, 255, 255, 180), 2));

    // 生成简单的波形数据（实际应该从音频文件获取）
    QVector<qreal> waveform;
    for (int i = 0; i < 20; ++i) {
        waveform.append(0.3 + 0.7 * qAbs(qSin(i * 0.5)));
    }

    // 绘制波形
    if (!waveform.isEmpty()) {
        int step = waveWidth / waveform.size();
        for (int i = 0; i < waveform.size(); ++i) {
            int x = waveLeft + i * step;
            int barHeight = waveform[i] * waveHeight;
            int y = waveTop + (waveHeight - barHeight) / 2;
            painter->drawLine(x, y, x, y + barHeight);
        }
    }

    // 绘制进度条背景
    painter->setPen(QPen(QColor(255, 255, 255, 80), 2));
    painter->drawLine(waveLeft, waveTop + waveHeight / 2,
                      waveLeft + waveWidth, waveTop + waveHeight / 2);

    painter->restore();
}

QString MessageDelegate::getAudioDuration(const QString &audioPath) const
{
    // 这里应该实现获取音频时长的逻辑
    // 简化版本：返回固定文本或从文件信息获取

    QFile file(audioPath);
    if (file.exists()) {
        qint64 fileSize = file.size();
        // 根据文件大小估算时长（WAV格式大致估算）
        if (fileSize > 0) {
            // WAV文件：文件大小 ≈ 44字节头 + 采样率 * 声道数 * 位深/8 * 秒数
            // 简化估算：假设是44.1kHz, 16bit, 单声道
            // 一秒采样44100,每次16比特，那么一秒就是44100 * 2字节= 88200字节
            // 文件大小 / 每秒字节 就是时长
            int estimatedSeconds = (fileSize - 44) / (44100 * 2);
            if (estimatedSeconds > 0) {
                int minutes = estimatedSeconds / 60;
                int seconds = estimatedSeconds % 60;
                return QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QLatin1Char('0'));
            }
        }
    }

    return "音频文件";
}


QString MessageDelegate::cleanFilePath(const QString &filePath) const
{
    if (filePath.startsWith("file://")) {
        return QUrl(filePath).toLocalFile();
    }
    return filePath;
}

void MessageDelegate::openFile(const QString &filePath) const
{
    QString cleanPath = cleanFilePath(filePath);

    if (!QFile::exists(cleanPath)) {
        return;
    }
    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

    if (mimeType.name().startsWith("video/")){
        openVideo(filePath);
    }else if (mimeType.name().startsWith("audio/")){
        openAudio(filePath);
    }else if (mimeType.name().startsWith("image/")){
        openImage(filePath);
    }
}

void MessageDelegate::openImage(const QString&filePath)const {
    QDialog *dlg = new QDialog;
    dlg->setStyleSheet("background:black;");
    dlg->setContentsMargins(0, 0, 0, 0);
    dlg->setWindowTitle("图片");

    QVBoxLayout *lay = new QVBoxLayout(dlg);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setAlignment(Qt::AlignCenter);

    QPixmap pix = SourceManager::GetInstance()->getPixmap(filePath);
    QLabel *label = new QLabel();
    label->setPixmap(pix.scaledToHeight(dlg->height()));

    // 限制最大尺寸，避免图片太大超出屏幕
    dlg->setMaximumSize(1200, 800);

    lay->addWidget(label);
    dlg->exec();  // 使用exec()确保对话框关闭前不会销毁
}

void MessageDelegate::openAudio(const QString &filePath) const
{
    QString cleanPath = cleanFilePath(filePath);

    qDebug() << "原始音频路径:" << filePath;
    qDebug() << "清理后音频路径:" << cleanPath;
    qDebug() << "文件是否存在:" << QFile::exists(cleanPath);
    qDebug() << "文件大小:" << QFileInfo(cleanPath).size() << "字节";

    if (!QFile::exists(cleanPath)) {
        qDebug() << "音频文件不存在:" << cleanPath;
        QMessageBox::warning(nullptr, "错误", "音频文件不存在或已被删除");
        return;
    }

    // 创建简单的音频播放对话框
    QDialog *audioDialog = new QDialog;
    audioDialog->setWindowTitle("播放音频");
    audioDialog->setFixedSize(350, 150);
    audioDialog->setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #2c3e50, stop:1 #34495e);
            color: white;
        }
        QPushButton {
            background: #3498db;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            min-width: 60px;
        }
        QPushButton:hover {
            background: #2980b9;
        }
        QPushButton:pressed {
            background: #21618c;
        }
        QPushButton:disabled {
            background: #7f8c8d;
            color: #bdc3c7;
        }
        QSlider::groove:horizontal {
            background: #34495e;
            height: 6px;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #ecf0f1;
            width: 16px;
            margin: -5px 0;
            border-radius: 8px;
        }
        QSlider::sub-page:horizontal {
            background: #3498db;
            border-radius: 3px;
        }
        QLabel {
            color: white;
            font-size: 12px;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(audioDialog);

    // 文件名显示
    QLabel *fileNameLabel = new QLabel(QFileInfo(cleanPath).fileName());
    fileNameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ecf0f1;");
    fileNameLabel->setAlignment(Qt::AlignCenter);

    // 进度条
    QSlider *progressSlider = new QSlider(Qt::Horizontal);
    progressSlider->setEnabled(false);

    // 时间显示
    QHBoxLayout *timeLayout = new QHBoxLayout;
    QLabel *currentTimeLabel = new QLabel("00:00");
    QLabel *totalTimeLabel = new QLabel("00:00");
    timeLayout->addWidget(currentTimeLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(totalTimeLabel);

    // 控制按钮
    QHBoxLayout *controlLayout = new QHBoxLayout;

    QPushButton *playButton = new QPushButton("播放");
    QPushButton *pauseButton = new QPushButton("暂停");
    QPushButton *stopButton = new QPushButton("停止");
    QPushButton *cancelButton = new QPushButton("取消");

    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);

    controlLayout->addStretch();
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(cancelButton);
    controlLayout->addStretch();

    layout->addWidget(fileNameLabel);
    layout->addWidget(progressSlider);
    layout->addLayout(timeLayout);
    layout->addLayout(controlLayout);

    // 音频播放器
    QMediaPlayer *player = new QMediaPlayer(audioDialog);
    QAudioOutput *audioOutput = new QAudioOutput(audioDialog);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(cleanPath));
    audioOutput->setVolume(0.8);

    // 播放按钮点击
    connect(playButton, &QPushButton::clicked, [=]() {
        player->play();
        playButton->setEnabled(false);
        pauseButton->setEnabled(true);
        stopButton->setEnabled(true);
        progressSlider->setEnabled(true);
    });

    // 暂停按钮点击
    connect(pauseButton, &QPushButton::clicked, [=]() {
        player->pause();
        playButton->setEnabled(true);
        pauseButton->setEnabled(false);
    });

    // 停止按钮点击
    connect(stopButton, &QPushButton::clicked, [=]() {
        player->stop();
        playButton->setEnabled(true);
        pauseButton->setEnabled(false);
        stopButton->setEnabled(false);
        progressSlider->setValue(0);
        currentTimeLabel->setText("00:00");
    });

    connect(cancelButton,&QPushButton::clicked,[=](){
        player->stop();
        audioDialog->reject();
    });

    // 更新进度
    connect(player, &QMediaPlayer::positionChanged, [=](qint64 position) {
        if (player->duration() > 0) {
            progressSlider->setValue(static_cast<int>((position * 100) / player->duration()));
            currentTimeLabel->setText(formatTime(position));
        }
    });

    // 总时长更新
    connect(player, &QMediaPlayer::durationChanged, [=](qint64 duration) {
        progressSlider->setRange(0, 100);
        totalTimeLabel->setText(formatTime(duration));
    });

    // 进度条跳转
    connect(progressSlider, &QSlider::sliderMoved, [=](int value) {
        if (player->duration() > 0) {
            player->setPosition((value * player->duration()) / 100);
        }
    });

    // 播放结束
    connect(player, &QMediaPlayer::playbackStateChanged, [=](QMediaPlayer::PlaybackState state) {
        if (state == QMediaPlayer::StoppedState) {
            playButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
            progressSlider->setValue(0);
            currentTimeLabel->setText("00:00");
        }
    });

    // 错误处理
    connect(player, &QMediaPlayer::errorOccurred, [=](QMediaPlayer::Error error, const QString &errorString) {
        qDebug() << "音频播放错误:" << errorString;
        QMessageBox::warning(audioDialog, "播放错误", "无法播放音频文件: " + errorString);
    });

    audioDialog->show();
    audioDialog->setAttribute(Qt::WA_DeleteOnClose);
}

QString MessageDelegate::formatTime(qint64 milliseconds) const
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}



void MessageDelegate::openVideo(const QString &videoPath) const
{
    // 创建简单的视频播放对话框
    QDialog *videoDialog = new QDialog;
    videoDialog->setContentsMargins(0,0,0,0);
    videoDialog->setWindowTitle("播放视频");
    videoDialog->resize(800, 600);
    videoDialog->setStyleSheet("QDialog {background:black;}");

    QVBoxLayout *layout = new QVBoxLayout(videoDialog);
    layout->setContentsMargins(0,0,0,0);

    QVideoWidget *videoWidget = new QVideoWidget;
    QMediaPlayer *player = new QMediaPlayer(videoDialog);

    // 设置音频输出
    QAudioOutput *audioOutput = new QAudioOutput(videoDialog);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8); // 80% 音量

    player->setVideoOutput(videoWidget);
    player->setSource(QUrl::fromLocalFile(videoPath));

    // 添加控制按钮
    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setAlignment(Qt::AlignHCenter);
    controlLayout->setContentsMargins(10,10,10,10);
    controlLayout->setSpacing(10);

    // 使用成员变量或智能指针来管理状态
    auto *playButton = new QPushButton;
    playButton->setObjectName("playButton");
    playButton->setProperty("status", "1"); // 初始状态为播放

    // 修复后的样式表
    playButton->setStyleSheet(R"(
        #playButton {
            border: none;
            background: transparent;
            min-width: 32px;
            min-height: 32px;
            max-width: 32px;
            max-height: 32px;
        }
        #playButton[status="1"] {
            border-image: url(:/Resources/main/stop.png);
        }
        #playButton[status="0"] {
            border-image: url(:/Resources/main/play.png);
        }
    )");

    auto *volumeButton = new QPushButton;
    volumeButton->setObjectName("volumeButton");
    volumeButton->setProperty("status", "1"); // 初始状态为有声音

    // 修复后的样式表
    volumeButton->setStyleSheet(R"(
        #volumeButton {
            border: none;
            background: transparent;
            min-width: 32px;
            min-height: 32px;
            max-width: 32px;
            max-height: 32px;
        }
        #volumeButton[status="1"] {
            border-image: url(:/Resources/main/volume.png);
        }
        #volumeButton[status="0"] {
            border-image: url(:/Resources/main/no-volume.png);
        }
    )");

    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(80);
    volumeSlider->setFixedWidth(100);

    // 修复信号槽连接 - 使用值捕获
    connect(playButton, &QPushButton::clicked, player, [player, playButton]() {
        QString currentStatus = playButton->property("status").toString();
        if (currentStatus == "1") {
            player->pause();
            playButton->setProperty("status", "0");
        } else {
            player->play();
            playButton->setProperty("status", "1");
        }
        // 刷新样式
        playButton->style()->unpolish(playButton);
        playButton->style()->polish(playButton);
        playButton->update();
    });

    connect(volumeButton, &QPushButton::clicked, player, [audioOutput, volumeButton, volumeSlider]() {
        QString currentVolumeStatus = volumeButton->property("status").toString();
        if (currentVolumeStatus == "1") {
            audioOutput->setMuted(true);
            volumeButton->setProperty("status", "0");
        } else {
            audioOutput->setMuted(false);
            volumeButton->setProperty("status", "1");
        }
        // 刷新样式
        volumeButton->style()->unpolish(volumeButton);
        volumeButton->style()->polish(volumeButton);
        volumeButton->update();
    });

    // 音量控制
    connect(volumeSlider, &QSlider::valueChanged, [audioOutput](int value) {
        audioOutput->setVolume(value / 100.0);
    });

    controlLayout->addStretch();
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(volumeSlider);
    controlLayout->addWidget(volumeButton);
    controlLayout->addStretch();

    layout->addWidget(videoWidget);
    layout->addLayout(controlLayout);

    // 连接错误处理
    connect(player, &QMediaPlayer::errorOccurred, [](QMediaPlayer::Error error, const QString &errorString) {
        qDebug() << "媒体播放错误:" << errorString;
    });

    connect(player, &QMediaPlayer::mediaStatusChanged, [](QMediaPlayer::MediaStatus status) {
        qDebug() << "媒体状态:" << status;
    });

    // 自动开始播放
    player->play();

    videoDialog->show();
    videoDialog->setAttribute(Qt::WA_DeleteOnClose);
}


ImageViewer::ImageViewer(const QPixmap &pix, QWidget *parent)
    : QWidget(parent), m_pix(pix)
{
    setWindowTitle("图片");
    setStyleSheet("background:black;");
    // 让窗口能够获取滚轮事件
    setFocusPolicy(Qt::StrongFocus);
}

void ImageViewer::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    // 居中 + 缩放
    int w = static_cast<int>(m_pix.width()  * m_scale);
    int h = static_cast<int>(m_pix.height() * m_scale);
    QRect target((width()  - w) / 2,
                 (height() - h) / 2,
                 w, h);
    p.drawPixmap(target, m_pix);
}

void ImageViewer::wheelEvent(QWheelEvent *ev)
{
    // 每滚一格 10% 缩放
    qreal delta = ev->angleDelta().y() > 0 ? m_scaleStep : -m_scaleStep;
    qreal newScale = qBound(m_minScale, m_scale + delta, m_maxScale);
    if (newScale != m_scale) {
        m_scale = newScale;
        update();   // 重绘
    }
    ev->accept();
}

