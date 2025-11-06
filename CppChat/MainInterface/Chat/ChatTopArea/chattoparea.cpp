#include "chattoparea.h"
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
#include <QTextEdit>
ChatTopArea::ChatTopArea(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

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

    headerLabelFromChat = new ClearAvatarLabel;
    headerLabelFromChat->setObjectName("headerLabelFromChat");
    headerLabelFromChat->setFixedSize({30,30});
    // 默认图片
    QPixmap pixmap(":/Resources/main/header.png");
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
}

void ChatTopArea::setupConnections()
{

    connect(this,&ChatTopArea::on_add_friend,friendAddDialog,&FriendAddDialog::do_add_friend);

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

void ChatTopArea::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        emit on_add_friend(this->searchBox->getContent());
        return ;
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
    painter.setPen(QPen(dotColor,3));
    painter.setBrush(QBrush(color));
    painter.drawRoundedRect(rect.adjusted(2,2,-2,-2),10,10);

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
    if (event->button() == Qt::LeftButton){
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
    if (!isPressed){
        isHovered = true;
        update();
        emit hover();
    }
    QLabel::enterEvent(event);
}

void StatusLabel::leaveEvent(QEvent *event)
{
    isHovered = false;
    isPressed = false;
    update();

    QLabel::leaveEvent(event);
}

void StatusLabel::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;
    update(); // 触发重绘恢复正常状态
    event->accept();
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
    resultList->setFixedHeight(0);  // 初始高度为0
    QTimer::singleShot(0, this, [this] {
        QWidget *central = window();           // 普通 QWidget 场景
        resultList->setParent(central);
        resultList->setWindowFlags(Qt::Widget);         // 变回普通子控件
        resultList->setFocusPolicy(Qt::StrongFocus);
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
    return searchEdit ? "" : searchEdit->text().trimmed();
}

void AnimatedSearchBox::do_search_clcked()
{
    toggleSearch();
}

void AnimatedSearchBox::do_text_changed(const QString &text)
{
    if (text.length() >= 2){
        updateResults(text);
        showResults();
    }else{
        hideResults();
    }
}

void AnimatedSearchBox::do_item_clicked(QListWidgetItem *item)
{

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
    if (resultList->count()==0)return;


    resultList->raise();

    /* 1. 取搜索框在屏幕中的绝对几何 */
    QRect r = searchEdit->rect();
    QPoint bottomLeft = searchEdit->mapToGlobal(r.bottomLeft());

    /* 2. 移到搜索框正下方 */
    resultList->move(bottomLeft);
    resultList->resize(searchEdit->width(), 150);

    resultList->show();
}

void AnimatedSearchBox::updateResults(const QString &keyword){
    resultList->clear();

    // 模拟搜索结果 - 实际项目中替换为真实数据
    QStringList mockUsers;
    if (keyword.contains("张")) {
        mockUsers << "张三 (ID: 10001)" << "张伟 (ID: 10004)" << "张小凡 (ID: 10007)";
    } else if (keyword.contains("李")) {
        mockUsers << "李四 (ID: 10002)" << "李华 (ID: 10005)";
    } else if (keyword.contains("王")) {
        mockUsers << "王五 (ID: 10003)" << "王明 (ID: 10006)";
    } else {
        // 通用搜索结果
        mockUsers << "张三 (ID: 10001)" << "李四 (ID: 10002)" << "王五 (ID: 10003)";
    }

    for (const QString &user : mockUsers) {
        QListWidgetItem *item = new QListWidgetItem(user);

        // 提取用户ID - 实际项目中从数据结构获取
        int userId = 10000 + resultList->count() + 1;
        item->setData(Qt::UserRole, userId);
        item->setData(Qt::UserRole + 1, user.split(" ").first());  // 用户名
        resultList->addItem(item);
    }

    // 如果没有结果
    if (resultList->count() == 0) {
        QListWidgetItem *item = new QListWidgetItem("未找到相关用户");
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setForeground(Qt::gray);
        resultList->addItem(item);
    }
    resultList->setFixedHeight(mockUsers.size()*30);
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
    connect(resultList,&QListWidget::itemClicked,this,&AnimatedSearchBox::on_item_clicked);
    connect(searchEdit,&QLineEdit::textChanged,this,&AnimatedSearchBox::do_text_changed);
    connect(resultList,&QListWidget::itemClicked,this,&AnimatedSearchBox::do_item_clicked);
    connect(searchEdit,&QLineEdit::returnPressed,[this](){
        emit on_search_clicked(searchEdit->text());
    });
}

bool AnimatedSearchBox::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && resultList->isVisible()) {
        QWidget *clickedWidget = qobject_cast<QWidget*>(obj);
        if (clickedWidget &&
            clickedWidget != resultList &&
            !resultList->isAncestorOf(clickedWidget) &&
            clickedWidget != searchEdit &&
            clickedWidget != searchButton) {
            // 点击了resultList、searchEdit、searchButton之外的区域
            hideResults();
        }
    }
    return QWidget::eventFilter(obj, event);
}

FriendAddDialog::FriendAddDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void FriendAddDialog::searchFriend(const QString &uid)
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

void FriendAddDialog::setUserUid(const QString &uid)
{
    this->userUid = uid;
}

void FriendAddDialog::setUserAvatar(const QString &avatar)
{
    this->userAvatar = avatar;
}

void FriendAddDialog::do_add_friend(const QString &uid)
{
    setUserUid(uid);
    searchFriend(uid);
    exec();
}




