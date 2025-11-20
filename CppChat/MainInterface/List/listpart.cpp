#include "listpart.h"
#include "../../../../Properties/signalrouter.h"

ListPart::ListPart(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

void ListPart::setupUI()
{
    setMinimumWidth(40);
    setMaximumWidth(220);

    stack = new QStackedWidget(this);
    friend_part = new FriendsListPart(stack);
    message_part = new MessagesListPart(stack);

    stack->addWidget(message_part);
    stack->addWidget(friend_part);

    // stack->set

}

void ListPart::setupConnections()
{
    connect(&SignalRouter::GetInstance(),&SignalRouter::on_change_list,this,&ListPart::do_change_list);
}

void ListPart::do_change_list(int index)
{
    if (index > stack->count()){
        return;
    }
    stack->setCurrentIndex(index);
}
