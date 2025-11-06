#include "tools.h"
#include "snipater.h"
#include <QPainter>
#include <QPaintEvent>
#include <QClipboard>
#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyleOption>
#include <QDir>
#include <QPainter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>


Tools::Tools(Snipater*s,QWidget *parent): QWidget{parent}
{
    sni = s;

    setMouseTracking(true);
    setFixedSize({90,20});

    this->_clipboard = QApplication::clipboard();

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    // setAttribute(Qt::WA_TranslucentBackground);

    createButtons();
}

void Tools::setPixmap(const QPixmap&pix)
{
    if(!pix.isNull()){
        qDebug() << "yes";
        this->_pixmap = pix;
        _clipboard->setPixmap(pix);
    }
}

void Tools::onSaveClicked()
{
    this->_pixmap = sni->grabRegion(sni->overlay->selectedRect());
    QString path = QFileDialog::getExistingDirectory(this,"选择存放目录",QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    QString name =  QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")+".png";
    this->_pixmap.save(QDir(path).filePath(name));
    hide();
    emit closeWindow();
}

void Tools::onCloseClicked()
{
    hide();
    emit closeWindow();
}

void Tools::onFinishClicked()
{
    sni->grabRegion(sni->overlay->selectedRect());
    hide();
    emit closeWindow();
}

void Tools::createButtons()
{
    _buttonContainer = new QWidget(this);
    _buttonContainer->setStyleSheet("background: transparent;");

    QHBoxLayout *_layout = new QHBoxLayout(_buttonContainer);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0,0,0,0);

    QPushButton*saveButton = new QPushButton(_buttonContainer);
    saveButton->setIcon(QIcon("/home/vivek/Codes/Qt/ScreenShot2/resources/save.png"));
    saveButton->setIconSize(QSize(24, 24));
    saveButton->setFixedSize({30,20});
    QPushButton*finishButton = new QPushButton(_buttonContainer);
    finishButton->setIcon(QIcon("/home/vivek/Codes/Qt/ScreenShot2/resources/yes.png"));
    finishButton->setIconSize(QSize(24, 24));
    finishButton->setFixedSize({30,20});
    QPushButton*closeButton = new QPushButton(_buttonContainer);
    closeButton->setIcon(QIcon("/home/vivek/Codes/Qt/ScreenShot2/resources/no.png"));
    closeButton->setIconSize(QSize(24, 24));
    closeButton->setFixedSize({30,20});

    connect(saveButton, &QPushButton::clicked, this, &Tools::onSaveClicked);
    connect(finishButton, &QPushButton::clicked, this, &Tools::onFinishClicked);
    connect(closeButton, &QPushButton::clicked, this, &Tools::onCloseClicked);


    _layout->addWidget(saveButton);
    _layout->addWidget(closeButton);
    _layout->addWidget(finishButton);

    _buttonContainer->setLayout(_layout);
}

