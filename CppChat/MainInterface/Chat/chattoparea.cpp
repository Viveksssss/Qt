#include "chattoparea.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
ChatTopArea::ChatTopArea(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

void ChatTopArea::setupUI()
{
    QHBoxLayout *main_hlay = new QHBoxLayout(this);
    main_hlay->setContentsMargins(0,0,0,0);
    main_hlay->setAlignment(Qt::AlignRight);
    main_hlay->setSpacing(10);

    statusLabel = new QLabel;
    statusLabel->setObjectName("statusLabel");
    statusLabel->setText("在线");
    statusLabel->setFixedSize({40,30});

    addFriendBtn = new QPushButton;
    addFriendBtn->setObjectName("addFriendBtn");
    addFriendBtn->setIcon(QIcon(":/Resources/main/add.png"));
    addFriendBtn->setIconSize({20,20});
    addFriendBtn->setFixedSize({30,30});

    newsBtn = new QPushButton;
    newsBtn->setObjectName("newsBtn");
    newsBtn->setIcon(QIcon(":/Resources/main/news.png"));
    newsBtn->setIconSize({20,20});
    newsBtn->setFixedSize({30,30});

    headerLabelFromChat = new QLabel;
    headerLabelFromChat->setObjectName("headerLabelFromChat");
    headerLabelFromChat->setText("头像");
    headerLabelFromChat->setFixedSize({40,30});

    foldBtn = new QPushButton;
    foldBtn->setObjectName("foldBtn");
    foldBtn->setIcon(QIcon(":/Resources/main/unfold.png"));
    foldBtn->setIconSize({20,20});
    foldBtn->setFixedSize({30,30});

    main_hlay->addWidget(statusLabel);
    main_hlay->addWidget(addFriendBtn);
    main_hlay->addWidget(newsBtn);
    main_hlay->addWidget(headerLabelFromChat);
    main_hlay->addWidget(foldBtn);
}

void ChatTopArea::setupConnections()
{

}

StatusLabel::StatusLabel(QWidget *parent)
{

}

void StatusLabel::setStatus(const QString &status)
{
    this->status = status;
    // 根据状态设置圆点颜色
    if (this->status == "在线") dotColor = QColor("#58f376");
    else if (this->status == "忙碌") dotColor = QColor("#e90739");
    else if (this->status == "离开") dotColor = QColor("#51615f");

    update(); // 触发重绘
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

    // 1. 圆角矩形
    painter.setPen(QPen(dotColor,2));
    painter.setBrush(QBrush(Qt::white));
    painter.drawRoundedRect(rect.adjusted(1,1,-1,-1),6,6);

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
