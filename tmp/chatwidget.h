// ChatWidget.h
#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QListView>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include "ChatMessage.h"
#include "chatmessagemodel.h"
#include "chatmessagedelegate.h"

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();

    void addMessage(const ChatMessage &message);
    void loadHistoryMessages(const QVector<ChatMessage> &messages);
    void clearChat();

private slots:
    void onMessageClicked(const QString &messageId);
    void onMessageDoubleClicked(const QString &messageId);
    void onCustomContextMenuRequested(const QPoint &pos);
    void deleteSelectedMessages();
    void copySelectedMessages();

private:
    void setupUI();
    void setupContextMenu();
    void scrollToBottom();

private:
    QListView *m_listView;
    ChatMessageModel *m_model;
    ChatMessageDelegate *m_delegate;
    QMenu *m_contextMenu;
    QAction *m_deleteAction;
    QAction *m_copyAction;
    QVector<QString> m_selectedMessages;
};

#endif // CHATWIDGET_H
