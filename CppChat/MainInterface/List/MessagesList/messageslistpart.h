#ifndef MESSAGESLISTPART_H
#define MESSAGESLISTPART_H


/******************************************************************************
 *
 * @file       messageslistpart.h
 * @brief      消息列表
 *
 * @author     Vivek
 * @date       2025/10/30
 * @history
 *****************************************************************************/

#include <QWidget>

class MessagesModel;
class MessageItemDelegate;
class QListView;
class QPushButton;
class QLabel;

struct UserInfo;
class MessageItem;
struct ConversationItem;
class MessagesListPart : public QWidget
{
    Q_OBJECT
    friend class MessagesItemDelegate;
public:
    explicit MessagesListPart(QWidget *parent = nullptr);
    QListView* getList();
private:
    void setupUI();
    void setupConnections();
    std::shared_ptr<ConversationItem> userFor(const QModelIndex&index);
signals:
    void on_loading_messages(); // to do_loading_messages

public slots:
    void do_loading_messages();
    void do_add_message_to_list(const ConversationItem&); // from TcpManager::on_add_message_to_list;
    void do_add_messages_to_list(const std::span<std::shared_ptr<ConversationItem>>&list); // from TcpManager::on_add_messages_to_list;
    void do_change_message_status(int,int);  // from MessagesNewsItem->SignalRouter::on_change_message_status;
    void do_change_peer(int);
    void do_get_message(const MessageItem&);    // from TcpManager::on_get_message
    void do_change_message_status(int peerUid,bool);  // from

private:
    QLabel *title;
    QPushButton *findButton;
    QListView *messagesList;
    MessagesModel *messagesModel;
    MessageItemDelegate *messagesDelegate;
    // 是否正在加载列表
    bool isLoading;

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
    inline bool getIsLoading()noexcept{return isLoading;}
    inline void setLoading(bool loading)noexcept{this->isLoading = loading;}
};

#endif // MESSAGESLISTPART_H
