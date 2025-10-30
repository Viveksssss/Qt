#include "friendslistpart.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

FriendsListPart::FriendsListPart(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
    setupConnections();
}

void FriendsListPart::setupUI()
{
    setMaximumWidth(170);

    QVBoxLayout *main_vlay = new QVBoxLayout(this);
    main_vlay->setContentsMargins(0,0,0,0);

    QHBoxLayout *top_hlay = new QHBoxLayout;
    top_hlay->setContentsMargins(15,0,15,0);

    // title
    title = new QLabel;
    title->setText("Message");
    auto font = title->font();
    font.setWeight(QFont::Black);
    title->setFont(font);
    top_hlay->addWidget(title);
    top_hlay->addStretch();

    // button
    pass = new QPushButton;
    top_hlay->addWidget(pass);




    main_vlay->addLayout(top_hlay);
    main_vlay->addWidget(nullptr);
}

void FriendsListPart::setupConnections()
{

}
