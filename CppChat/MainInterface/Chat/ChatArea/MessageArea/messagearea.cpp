#include "messagearea.h"
#include <QListView>
#include <QVBoxLayout>
#include <QScrollBar>
#include "./messagemodel.h"
#include "./messagedelegate.h"

MessageArea::MessageArea(QWidget *parent)
    : QWidget{parent}
    , list(new QListView(this))
    , delegate(new MessageDelegate(this))
    , model(new MessageModel(this))
{
    setupUI();
    setupConnections();
}

void MessageArea::setupUI()
{
    QVBoxLayout *main_vLay = new QVBoxLayout(this);
    main_vLay->setContentsMargins(0,0,0,0);
    main_vLay->setSpacing(0);
    main_vLay->addWidget(list);

    list->setObjectName("messageList");
    list->setModel(model);
    list->setItemDelegate(delegate);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    list->setContextMenuPolicy(Qt::CustomContextMenu);
    list->setAlternatingRowColors(false);
    // list->setSpacing(5);
    list->verticalScrollBar()->setSingleStep(20);
    list->setUniformItemSizes(false);   // 必须 false，才允许每行高不同
    list->setWordWrap(true);            // 内部也会触发 QTextLayout 折行
    list->setSelectionMode(QAbstractItemView::NoSelection);
    list->setFocusPolicy(Qt::ClickFocus);
}

void MessageArea::setupConnections()
{
}

MessageModel *MessageArea::getModel()
{
    return this->model;
}

void MessageArea::do_area_to_bottom()
{
    list->scrollToBottom();
}

void MessageArea::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

}
