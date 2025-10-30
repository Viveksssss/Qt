#include "sidebarpart.h"
#include "../stylemanager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>

SideBarPart::SideBarPart(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

void SideBarPart::setupUI()
{
    setMinimumWidth(60);
    setMaximumWidth(60);

    setFocusPolicy(Qt::ClickFocus);   // 允许鼠标点击时自己拿焦点

    layout =  new QVBoxLayout(this);
    layout->setContentsMargins(-10,0,0,0);
    layout ->setAlignment(Qt::AlignTop);
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true); // 互斥选择

    addItem("message", "消息", ":/Resources/main/message.png");
    addItem("friends", "好友", ":/Resources/main/friends.png");
}

void SideBarPart::setupConnections()
{

}


void SideBarPart::addItem(const QString &id, const QString &text, const QString &icon)
{
    SideBarItem item{id,text,icon,static_cast<int>(items.size())};
    items.append(item);
    createButton(item);
}

void SideBarPart::removeItem(const QString &id)
{
    items.erase(std::remove_if(items.begin(),items.end(),[&id](SideBarItem item){
        return id == item.id;
    }));
}

void SideBarPart::createButton(const SideBarItem &item,bool showText)
{
    QPushButton *btn = new QPushButton;
    btn->setFixedWidth(45);

    // 设置按钮文本和图标
    if (!item.icon.isEmpty()) {
        btn->setIcon(QIcon(item.icon));
        btn->setIconSize(QSize(20, 20));
    }
    if(showText){
        btn->setText(item.text);
    }
    btn->clearFocus();

    // 设置样式
    btn->setStyleSheet(R"(
        QPushButton {
            text-align: left;
            padding: 8px 12px;
            border: none;
            background-color: transparent;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #c7e4fa;
            border-radius:15px;
        }
        QPushButton:checked {
            background-color: #5dd3fe;
            border-radius:15px;
            color: white;
        }
    )");

    btn->setCheckable(true);
    btn->setProperty("itemId", item.id); // 存储标识

    // 添加到布局和按钮组
    layout->insertWidget(layout->count() - 1, btn); // 插入到弹簧之前
    buttonGroup->addButton(btn);

    buttons[item.id] = btn;

    // 连接信号
    connect(btn, &QPushButton::clicked, this, [this,btn, id = item.id]() {
        emit on_sidebar_btn_clicked(id);
    });
}
