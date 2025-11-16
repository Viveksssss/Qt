#include "chattoparea.h"
#include "../../../Properties/sourcemanager.h"
#include "../../../usermanager.h"
#include "../../../tcpmanager.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QListWidget>
#include <QApplication>
#include <QTimer>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTextEdit>
#include <QScrollBar>
ChatTopArea::ChatTopArea(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

ChatTopArea::~ChatTopArea()= default;

void ChatTopArea::setupUI()
{
    friendAddDialog = new FriendAddDialog(this);

    QHBoxLayout *main_hlay = new QHBoxLayout(this);
    main_hlay->setContentsMargins(0,0,0,0);
    main_hlay->setAlignment(Qt::AlignRight);
    main_hlay->setSpacing(10);

    statusLabel = new StatusLabel(this);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setStatus("在线");
    statusLabel->setFixedSize({60,30});

    searchBox = new AnimatedSearchBox;
    searchBox->setMaximumWidth(300);

    newsBtn = new QPushButton;
    newsBtn->setObjectName("newsBtn");
    newsBtn->setIcon(QIcon(":/Resources/main/news.png"));
    newsBtn->setIconSize({20,20});
    newsBtn->setFixedSize({30,30});

    redDot = new QLabel(newsBtn);
    redDot->setVisible(false);
    redDot->setFixedSize(10, 10);
    redDot->setStyleSheet("background: red; border-radius: 5px;");
    redDot->move(newsBtn->width() - 10, 2); // 右上角位置
    redDot->setAttribute(Qt::WA_TransparentForMouseEvents); // 穿透鼠标事

    headerLabelFromChat = new ClearAvatarLabel;
    headerLabelFromChat->setObjectName("headerLabelFromChat");
    headerLabelFromChat->setFixedSize({30,30});
    // 默认图片
    QPixmap pixmap(":/Resources/main/header-default.png");
    headerLabelFromChat->setPixmap(pixmap);
    headerLabelFromChat->setScaledContents(true);

    foldBtn = new QPushButton;
    foldBtn->setObjectName("foldBtn");
    foldBtn->setIcon(QIcon(":/Resources/main/unfold.png"));
    foldBtn->setIconSize({20,20});
    foldBtn->setFixedSize({30,30});

    main_hlay->addWidget(statusLabel);
    main_hlay->addWidget(searchBox);
    main_hlay->addWidget(newsBtn);
    main_hlay->addWidget(headerLabelFromChat);
    main_hlay->addWidget(foldBtn);

    newsPanel = new NotificationPanel(this);
    newsPanel->setObjectName("NotificationPanel");
    newsPanel->setFixedWidth(250);
    newsPanel->hide();

    qApp->installEventFilter(this);
}

void ChatTopArea::setupConnections()
{

    connect(this,&ChatTopArea::on_search_friend,searchBox,&AnimatedSearchBox::do_text_changed);

    connect(newsBtn,&QPushButton::clicked,this,&ChatTopArea::do_show_news);

    connect(newsPanel,&NotificationPanel::on_unshow_red_dot,this,&ChatTopArea::do_unshow_red_dot);

    connect(newsPanel,&NotificationPanel::on_show_red_dot,this,&ChatTopArea::do_show_red_dot);

    // 在按钮点击的槽函数中
    connect(statusLabel, &StatusLabel::clicked, this, [this]() {
        // 创建菜单
        QMenu *menu = new QMenu(this);
        menu->setWindowFlags(Qt::Popup);  // 设置为弹出窗口
        menu->setStyleSheet(
            "QMenu {"
            "    background-color: white;"
            "    border: 1px solid #e0e0e0;"
            "    border-radius: 6px;"
            "    padding: 4px;"
            "}"
            "QMenu::item {"
            "    padding: 6px 12px;"
            "    border-radius: 4px;"
            "    margin: 2px;"
            "}"
            "QMenu::item:selected {"
            "    background-color: #e3f2fd;"
            "}"
            );

        // 添加状态选项
        QAction *onlineAction = menu->addAction("🟢 在线");
        QAction *busyAction = menu->addAction("🔴 忙碌");
        QAction *offlineAction = menu->addAction("⚫ 离线");

        // 连接菜单项点击信号
        connect(onlineAction, &QAction::triggered, this, [this]() {
            statusLabel->setStatus("在线");
        });
        connect(busyAction, &QAction::triggered, this, [this]() {
            statusLabel->setStatus("忙碌");
        });
        connect(offlineAction, &QAction::triggered, this, [this]() {
            statusLabel->setStatus("离线");
        });

        // 在按钮下方显示菜单
        QPoint pos = statusLabel->mapToGlobal(QPoint(0, statusLabel->height()));
        menu->exec(pos);

        // 菜单关闭后自动删除
        menu->setAttribute(Qt::WA_DeleteOnClose);
    });
}

void ChatTopArea::do_show_news()
{
    newsPanel->showPanel();
}

void ChatTopArea::do_show_red_dot()
{
    redDot->setVisible(true);
}

void ChatTopArea::do_unshow_red_dot()
{
    redDot->setVisible(false);
}

void ChatTopArea::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        if(!padding){
            padding = true;
            QTimer::singleShot(1000,[this]()mutable{
                this->padding = false;
            });
            emit on_search_friend(this->searchBox->getContent());
        }
        return;
    }
    else{
        QWidget::keyPressEvent(event);
    }
}



StatusLabel::StatusLabel(QWidget *parent)
{}

void StatusLabel::setStatus(const QString &status)
{
    this->status = status;
    // 根据状态设置圆点颜色
    if (this->status == "在线") dotColor = QColor(0x58f376);
    else if (this->status == "忙碌") dotColor = QColor(0xe90739);
    else if (this->status == "离线") dotColor = Qt::gray;

    update(); // 触发重绘
}

QString StatusLabel::getStatus()
{
    return status;
}

void StatusLabel::setDotColor(const QColor &color)
{
    dotColor = color;
    update();
}

void StatusLabel::setEnabled(bool enabled)
{
    isEnabled = enabled;
}

void StatusLabel::setShowBorder(bool show) noexcept
{
    this->showBorder = show;
}

void StatusLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    QColor color = Qt::white;
    if(isPressed){
        color = QColor("#f7f8e7");
    }else if (isHovered){
        color = QColor("#f4d4f7");
    }

    // 1. 圆角矩形
    if (showBorder){
        painter.setPen(QPen(dotColor,3));
        painter.setBrush(QBrush(color));
        painter.drawRoundedRect(rect.adjusted(2,2,-2,-2),10,10);
    }

    // 2. 绘制左侧圆点
    int dotSize = 8;
    int dotMargin = 8;
    QRect dotRect(dotMargin,
                  (rect.height() - dotSize) / 2,
                  dotSize, dotSize);

    painter.setPen(Qt::NoPen);
    painter.setBrush(dotColor);
    painter.drawEllipse(dotRect);

    // 3. 绘制右侧文字
    painter.setPen(Qt::black); // 黑色文字
    QFont font = painter.font();
    font.setPointSize(9);
    font.setBold(false);
    painter.setFont(font);

    QRect textRect(dotRect.right() + 6, 0,
                   rect.width() - dotRect.right() - 12, rect.height());
    painter.drawText(textRect, Qt::AlignCenter, status);
}

void StatusLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled){
        isPressed = true;
        emit clicked();
        update();
        event->accept();
    }else{
        QLabel::mousePressEvent(event);
    }
}

void StatusLabel::enterEvent(QEnterEvent *event)
{
    if (!isPressed && isEnabled){
        isHovered = true;
        update();
        emit hover();
    }
    QLabel::enterEvent(event);
}

void StatusLabel::leaveEvent(QEvent *event)
{
    if (isEnabled){
        isHovered = false;
        isPressed = false;
        update();
    }

    QLabel::leaveEvent(event);
}

void StatusLabel::mouseReleaseEvent(QMouseEvent *event)
{

    if (isEnabled){
        isPressed = false;
        update(); // 触发重绘恢复正常状态
        event->accept();
    }
}


AnimatedSearchBox::AnimatedSearchBox(QWidget *parent)
    : isExpanded(false)
{
    setupUI();
    setupConnections();
    qApp->installEventFilter(this);   // 监听整个程序
}

void AnimatedSearchBox::setupUI()
{
    setMinimumWidth(30);
    setMaximumWidth(200);
    setFixedHeight(30);
    setAttribute(Qt::WA_TranslucentBackground);


    QVBoxLayout *main_vlay = new QVBoxLayout(this);
    main_vlay->setContentsMargins(0,0,0,0);
    main_vlay->setSpacing(5);

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->setAlignment(Qt::AlignRight);
    searchLayout->setContentsMargins(0,0,0,0);

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Enter Name or ID");
    searchEdit->setFixedHeight(30);
    searchEdit->setObjectName("searchEdit");
    searchEdit->setMaxLength(25);
    searchEdit->hide(); // 默认隐藏
    clearAction = new QAction;
    clearAction->setObjectName("clearAction");
    clearAction->setIcon(QIcon(":/Resources/main/clear.png"));

    searchEdit->addAction(clearAction,QLineEdit::TrailingPosition);
    connect(clearAction,&QAction::triggered,this,[this]{
        searchEdit->clear();
    });

    searchButton = new QToolButton;
    searchButton->setVisible(true);
    searchButton->setIcon(QIcon(":/Resources/main/add.png"));
    searchButton->setIconSize({20,20});
    searchButton->setFixedSize({30,30});
    searchButton->setToolTip("Search");
    searchButton->setObjectName("searchButton");
    searchButton->show();

    resultList = new QListWidget(window());
    resultList->setObjectName("resultList");
    resultList->setFixedSize(380, 320);  // 使用 setFixedSize
    resultList->setUniformItemSizes(true);
    resultList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    resultList->setVerticalScrollMode(QListWidget::ScrollPerItem); // 对滚轮无效。。。
    resultList->viewport()->installEventFilter(this); // 手写过滤器，每次滚动一项
    resultList->hide();
    resultList->setSpacing(1);
    QTimer::singleShot(0, this, [this] {
        QWidget *central = window();           // 普通 QWidget 场景
        resultList->setParent(central);
        resultList->setWindowFlags(Qt::Popup);         // 变回普通子控件
    });

    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchButton);

    main_vlay->addLayout(searchLayout);

    // 动画
    animation = new QPropertyAnimation(this,"searchWidth");
    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::OutCurve);

    opacityEffect = new QGraphicsOpacityEffect(searchEdit);
    opacityEffect->setOpacity(0.0);
    searchEdit->setGraphicsEffect(opacityEffect);
}

void AnimatedSearchBox::toggleSearch()
{
    isExpanded = !isExpanded;
    if(!isExpanded){
        animation->setStartValue(200);
        animation->setEndValue(0);
        hideResults();
    }else{
        searchEdit->show();
        animation->setStartValue(0);
        animation->setEndValue(200);
    }
    startAnimation();
}

QString AnimatedSearchBox::getContent()
{
    return !searchEdit ? "" : searchEdit->text().trimmed();
}

void AnimatedSearchBox::do_search_clcked()
{
    toggleSearch();
}

void AnimatedSearchBox::do_text_changed(const QString &text)
{
    if (text.length() >= 1){
        getSearchUsers(text.trimmed());
        // updateResults();
        // showResults();
    }else{
        hideResults();
    }
}

void AnimatedSearchBox::do_users_searched(QList<std::shared_ptr<UserInfo>>list)noexcept
{
    this->usersList = std::move(list);
    updateResults();
    showResults();
}


void AnimatedSearchBox::setSearchWidth(int width)
{
    textWidth = width;
    searchEdit->setFixedWidth(width);
}

void AnimatedSearchBox::hideResults()
{
    resultList->hide();
    resultList->clear();
}

void AnimatedSearchBox::showResults()
{
    if (resultList->count() == 0) {
        return;
    }

    if (!resultList->parent()) {
        resultList->setParent(window());
    }

    QRect r = searchEdit->rect();
    QPoint bottomLeft = searchEdit->mapToGlobal(r.bottomLeft());
    bottomLeft.setX(bottomLeft.x()-80);
    bottomLeft.setY(bottomLeft.y()+5);


    resultList->move(bottomLeft);
    resultList->show();
    resultList->raise();

    // 强制更新
    // resultList->update();
    // resultList->repaint();
}

void AnimatedSearchBox::updateResults(){
    resultList->clear();
    for (const std::shared_ptr<UserInfo> &user : this->usersList) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(QSize(350,40));
        // 提取用户ID - 实际项目中从数据结构获取
        qDebug() <<
                 "-----------------------------"<< user->avatar;
        FriendsItem *friendItem = new FriendsItem(user->id,user->avatar,user->name,user->sex,user->status);
        resultList->addItem(item);
        resultList->setItemWidget(item,friendItem);
    }

    if (resultList->count() == 0) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(QSize(350,40));
        // 提取用户ID - 实际项目中从数据结构获取
        item->setText("未查询到用户");
        item->setTextAlignment(Qt::AlignCenter);
        resultList->addItem(item);
    }

}

void AnimatedSearchBox::getSearchUsers(const QString &uid)
{
    QJsonObject obj;
    obj["fromUid"] = UserManager::GetInstance()->GetUid();
    obj["toUid"] = uid.trimmed();

    QJsonDocument doc(obj);

    emit TcpManager::GetInstance()->on_send_data(RequestType::ID_SEARCH_USER_REQ,doc.toJson(QJsonDocument::Compact));
}

void AnimatedSearchBox::startAnimation()
{
    animation->start();

    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    opacityAnimation->setDuration(300);
    opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);

    if (isExpanded) {
        opacityAnimation->setStartValue(0.0);
        opacityAnimation->setEndValue(1.0);
        searchEdit->setFocus();
        clearAction->setVisible(true);
        showResults();
    } else {
        opacityAnimation->setStartValue(1.0);
        opacityAnimation->setEndValue(0.0);
        connect(opacityAnimation, &QPropertyAnimation::finished, [this]() {
            searchEdit->hide();
            searchEdit->clear();
            clearAction->setVisible(false);
        });
    }

    opacityAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimatedSearchBox::setupConnections()
{
    connect(searchButton,&QToolButton::clicked,this,&AnimatedSearchBox::do_search_clcked);

    connect(searchEdit,&QLineEdit::returnPressed,[this](){
        emit on_search_clicked(searchEdit->text());
    });

    connect(TcpManager::GetInstance().get(),&TcpManager::on_users_searched,this,&AnimatedSearchBox::do_users_searched);
}

bool AnimatedSearchBox::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && resultList->isVisible()) {
        QWidget *clickedWidget = qobject_cast<QWidget*>(obj);
        if (!clickedWidget) {
            return QWidget::eventFilter(obj, event);
        }

        // 获取全局鼠标位置
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint globalPos = mouseEvent->globalPos();

        // 检查点击是否在resultList区域内
        QRect resultListRect = resultList->geometry();
        if (resultListRect.contains(globalPos)) {
            return QWidget::eventFilter(obj, event);
        }

        // 检查点击是否在searchEdit区域内
        // QRect searchEditRect = searchEdit->geometry();
        // searchEditRect.moveTopLeft(searchEdit->mapToGlobal(QPoint(0, 0)));
        // if (searchEditRect.contains(globalPos)) {
        //     return QWidget::eventFilter(obj, event);
        // }

        // 如果都不在，隐藏结果
        hideResults();
    }else if(event->type() == QEvent::Wheel && obj == resultList->viewport()){
        QWheelEvent*wheelEvent = static_cast<QWheelEvent*>(event);
        int delta = wheelEvent->angleDelta().y();
        if (delta == 0){
            return false;
        }

        if (delta > 0) {
            // 向上滚动一个项
            resultList->verticalScrollBar()->setValue(resultList->verticalScrollBar()->value() - 1);
        } else {
            // 向下滚动一个项
            resultList->verticalScrollBar()->setValue(resultList->verticalScrollBar()->value() + 1);
        }
        return true;
    }
    return QWidget::eventFilter(obj, event);
}


FriendAddDialog::FriendAddDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void FriendAddDialog::searchFriend(int uid)
{
    QPixmap avatar(userAvatar);
    avatarLabel->setPixmap(avatar.scaled(50,50));
}

void FriendAddDialog::setupUI()
{
    // 默认头像
    userAvatar = ":/Resources/main/header.png";

    // 设置对话框属性 - 只调用一次
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    setObjectName("friendAddDialog");
    setFixedSize(320, 320); // 增加高度以适应备注框
    // 移除了重复的 setWindowFlags

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5); // 减少间距
    mainLayout->setContentsMargins(10,0,10,10); // 调整边距

    // 标题
    titleLabel = new QLabel("添加好友");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 用户信息区域（圆角框）
    QFrame *userInfoFrame = new QFrame();
    userInfoFrame->setObjectName("userInfoFrame");
    userInfoFrame->setFixedHeight(80);

    QHBoxLayout *userLayout = new QHBoxLayout(userInfoFrame);
    userLayout->setContentsMargins(15, 10, 15, 10);
    userLayout->setSpacing(15);

    // 头像 - 设置图片
    avatarLabel = new QLabel();
    avatarLabel->setObjectName("avatarLabel");
    avatarLabel->setFixedSize(50, 50);
    QPixmap avatarPixmap(userAvatar);
    if (!avatarPixmap.isNull()) {
        avatarLabel->setPixmap(avatarPixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // 设置默认样式
        avatarLabel->setStyleSheet("background-color: #dee2e6; border-radius: 8px;");
    }

    // 姓名和UID区域 - 使用占位文本
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);
    infoLayout->setAlignment(Qt::AlignVCenter);

    nameLabel = new QLabel("用户名"); // 使用占位文本
    nameLabel->setObjectName("nameLabel");

    uidLabel = new QLabel("UID: "); // 使用占位文本
    uidLabel->setObjectName("uidLabel");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(uidLabel);

    userLayout->addWidget(avatarLabel);
    userLayout->addLayout(infoLayout);
    userLayout->addStretch();

    mainLayout->addWidget(userInfoFrame);

    // 备注输入区域
    QLabel *remarkLabel = new QLabel("备注");
    remarkLabel->setObjectName("remarkLabel");
    mainLayout->addWidget(remarkLabel);

    remarkEdit = new QTextEdit();
    remarkEdit->setObjectName("remarkEdit");
    remarkEdit->setFixedHeight(60);
    remarkEdit->setPlaceholderText("请输入备注信息（可选）");
    mainLayout->addWidget(remarkEdit);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10); // 减少按钮间距

    addButton = new QPushButton("添加");
    addButton->setObjectName("addButton");
    addButton->setFixedSize(80, 32);

    cancelButton = new QPushButton("取消");
    cancelButton->setObjectName("cancelButton");
    cancelButton->setFixedSize(80, 32);

    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(cancelButton, &QPushButton::clicked, this, &FriendAddDialog::reject);
    connect(addButton, &QPushButton::clicked, this, &FriendAddDialog::accept);
}
void FriendAddDialog::setUserName(const QString &name)
{
    this->userName = name;
}

void FriendAddDialog::setUserUid(int uid)
{
    this->userUid = uid;
}

void FriendAddDialog::setUserAvatar(const QString &avatar)
{
    this->userAvatar = avatar;
}

void FriendAddDialog::do_add_friend(int uid)
{
    setUserUid(uid);
    searchFriend(uid);
    exec();
}


FriendsItem::FriendsItem(int uid, const QString &avatar, const QString &name,int sex,int status,QWidget*parent)
    : QWidget(parent)
    , _uid(uid)
    , _icon(avatar)
    , _name(name)
    , _status(status)
    , _sex(sex)
{
    setupUI();
    setupConnections();
}

void FriendsItem::setupUI()
{
    QHBoxLayout*main_hlay = new QHBoxLayout(this);
    main_hlay->setContentsMargins(10,0,10,0);
    main_hlay->setSpacing(5);

    _avatar = new QLabel;
    _avatar->setFixedSize(44, 44);  // 与finalPixmap尺寸一致
    _avatar->setAlignment(Qt::AlignCenter);  // 关键：内容居中

    // 使用原来的数据加载逻辑
    QPixmap originalPixmap;
    // 创建带边框的圆形图片
    if (_icon.startsWith(":/")){
        originalPixmap = QPixmap(_icon);
    }else{
        QByteArray imageData = QByteArray::fromBase64(_icon.toUtf8());
        originalPixmap.loadFromData(imageData);
    }

    // 使用原来的绘制逻辑，改小尺寸
    QPixmap finalPixmap(36, 36);
    finalPixmap.fill(Qt::transparent);

    QPainter painter(&finalPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 1. 先绘制边框
    QColor borderColor = (_sex == 1) ? QColor("#00F5FF") : QColor("#FF69B4");
    painter.setBrush(borderColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 36, 36);

    // 2. 绘制背景
    painter.setBrush(QColor("#E3F2FD"));
    painter.drawEllipse(2, 2, 32, 32);  // 边框内部

    // 3. 裁剪并绘制头像
    QPainterPath clipPath;
    clipPath.addEllipse(2, 2, 32, 32);  // 头像区域
    painter.setClipPath(clipPath);
    painter.drawPixmap(2, 2, 32, 32, originalPixmap.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    // 设置最终的头像图片
    _avatar->setPixmap(finalPixmap);
    _avatar->setAlignment(Qt::AlignCenter);  // 关键：内容居中


    QLabel*name = new QLabel;
    name->setText(_name);
    QFont font = name->font();
    font.setBold(true);
    font.setPointSize(10);
    QPalette plt = name->palette();
    plt.setColor(QPalette::WindowText,QColor(333333));
    name->setFont(font);
    name->setPalette(plt);
    name->setAlignment(Qt::AlignCenter);

    _statusLabel = new StatusLabel;
    _statusLabel->setStatus(_status==0?"离线":"在线");
    _statusLabel->setEnabled(false);
    _statusLabel->setFixedSize({60,30});
    _statusLabel->setShowBorder(false);

    _applyFriend = new QPushButton;
    if (_uid == UserManager::GetInstance()->GetUid()){
        _applyFriend->setText("已添加");
        _applyFriend->setEnabled(false);    // 不允许自己添加自己。
    }else{
        _applyFriend->setText("添加");
    }
    _applyFriend->setFixedSize({60,30});
    _applyFriend->setStyleSheet(R"(
        QPushButton {
            background-color: #6bb9ef;

            color: #ffffff;
            border: none;
            border-radius: 10px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3fd9d4;
        }
    )");

    main_hlay->addWidget(_avatar,Qt::AlignCenter);
    main_hlay->addSpacing(5);
    main_hlay->addWidget(name);
    main_hlay->addStretch();
    main_hlay->addWidget(_statusLabel);
    main_hlay->addWidget(_applyFriend);
}

void FriendsItem::setupConnections()
{
    connect(_applyFriend,&QPushButton::clicked,this,[this](bool){
        QJsonObject obj;
        obj["fromUid"] = static_cast<int>(UserManager::GetInstance()->GetUid());
        obj["fromName"] = UserManager::GetInstance()->GetName();
        obj["fromEmail"] = UserManager::GetInstance()->GetEmail();
        obj["fromDesc"] = UserManager::GetInstance()->GetDesc();
        obj["fromSex"] = UserManager::GetInstance()->GetSex();
        if (UserManager::GetInstance()->GetIcon().startsWith(":/")){
            obj["fromIcon"] = UserManager::GetInstance()->pixmapToBase64(UserManager::GetInstance()->GetIcon());
        }else{
            obj["fromIcon"] = UserManager::GetInstance()->GetIcon();
        }

        obj["toUid"] = this->_uid; // 对方的uid
        qDebug() << "fromUid" << obj["fromUid"] << "\t" << "toUid" << this->_uid;

        QJsonDocument doc;
        doc.setObject(obj);
        QByteArray data = doc.toJson(QJsonDocument::Compact);

        emit TcpManager::GetInstance()->on_send_data(RequestType::ID_ADD_FRIEND_REQ,data);
        this->_applyFriend->setEnabled(false);
        showToolTip(_applyFriend,"已发送好友请求");
    });
}

void FriendsItem::setShowBorder(bool show) noexcept
{
    _statusLabel->setShowBorder(show);
}
