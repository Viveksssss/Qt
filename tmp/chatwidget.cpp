// ChatWidget.cpp
#include "chatwidget.h"
#include <QScrollBar>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , m_listView(new QListView(this))
    , m_model(new ChatMessageModel(this))
    , m_delegate(new ChatMessageDelegate(this))
    , m_contextMenu(new QMenu(this))
    , m_deleteAction(new QAction("删除", this))
    , m_copyAction(new QAction("复制", this))
{
    setupUI();
    setupContextMenu();

    connect(m_delegate, &ChatMessageDelegate::messageClicked,
            this, &ChatWidget::onMessageClicked);
    connect(m_delegate, &ChatMessageDelegate::messageDoubleClicked,
            this, &ChatWidget::onMessageDoubleClicked);
    connect(m_listView, &QListView::customContextMenuRequested,
            this, &ChatWidget::onCustomContextMenuRequested);
    connect(m_deleteAction, &QAction::triggered,
            this, &ChatWidget::deleteSelectedMessages);
    connect(m_copyAction, &QAction::triggered,
            this, &ChatWidget::copySelectedMessages);
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 设置ListView
    m_listView->setModel(m_model);
    m_listView->setItemDelegate(m_delegate);
    m_listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    layout->addWidget(m_listView);
}

void ChatWidget::setupContextMenu()
{
    m_contextMenu->addAction(m_copyAction);
    m_contextMenu->addAction(m_deleteAction);
}

void ChatWidget::addMessage(const ChatMessage &message)
{
    m_model->addMessage(message);
    scrollToBottom();
}

void ChatWidget::loadHistoryMessages(const QVector<ChatMessage> &messages)
{
    m_model->clearMessages();
    for (const auto &message : messages) {
        m_model->addMessage(message);
    }
    scrollToBottom();
}

void ChatWidget::clearChat()
{
    m_model->clearMessages();
    m_selectedMessages.clear();
}

void ChatWidget::onMessageClicked(const QString &messageId)
{
    // 切换消息的选中状态
    ChatMessage message = m_model->getMessage(messageId);
    if (!message.id().isEmpty()) {
        bool newSelectedState = !message.isSelected();
        m_model->setMessagesSelected({messageId}, newSelectedState);

        if (newSelectedState) {
            m_selectedMessages.append(messageId);
        } else {
            m_selectedMessages.removeAll(messageId);
        }
    }
}

void ChatWidget::onMessageDoubleClicked(const QString &messageId)
{
    // 处理双击事件，比如预览图片、播放视频等
    ChatMessage message = m_model->getMessage(messageId);
    // 根据消息类型执行相应操作
}

void ChatWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_listView->indexAt(pos);
    if (index.isValid()) {
        m_contextMenu->exec(m_listView->mapToGlobal(pos));
    }
}

void ChatWidget::deleteSelectedMessages()
{
    for (const QString &messageId : m_selectedMessages) {
        m_model->removeMessage(messageId);
    }
    m_selectedMessages.clear();
}

void ChatWidget::copySelectedMessages()
{
    // 实现复制选中消息的逻辑
    QString copiedText;
    for (const QString &messageId : m_selectedMessages) {
        ChatMessage message = m_model->getMessage(messageId);
        copiedText += message.content() + "\n";
    }
    // 将文本复制到剪贴板
    // QApplication::clipboard()->setText(copiedText);
}

void ChatWidget::scrollToBottom()
{
    QScrollBar *scrollBar = m_listView->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
