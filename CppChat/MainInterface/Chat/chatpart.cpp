#include "chatpart.h"
#include "chattoparea.h"
#include "chatarea.h"
#include <QVBoxLayout>
#include <QLabel>

ChatPart::ChatPart(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

void ChatPart::setupUI()
{
    setContentsMargins(0,0,0,0);
    setMinimumWidth(200);
    setMinimumHeight(200);
    setFocusPolicy(Qt::ClickFocus);

    QVBoxLayout *main_vlay = new QVBoxLayout(this);
    main_vlay->setContentsMargins(10,10,10,10);

    // chatTopArea
    chatTopArea = new ChatTopArea;
    main_vlay->addWidget(chatTopArea);

    // chatArea
    chatArea = new ChatArea;
    main_vlay->addWidget(chatArea);
}

void ChatPart::setupConnections()
{

}
