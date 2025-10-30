#include "chatarea.h"

ChatArea::ChatArea(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

void ChatArea::setupUI()
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setStyleSheet("background-color: red;");
    setAttribute(Qt::WA_StyledBackground);
}

void ChatArea::setupConnections()
{

}
