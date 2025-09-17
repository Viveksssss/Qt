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
    setAttribute(Qt::WA_Hover);  // 启用悬停事件

    this->_clipboard = QApplication::clipboard();

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet(R"(
        QWidget {
            background: rgba(45, 45, 45, 220);
            border: 1px solid #666666;
            border-radius: 6px;
        }
        QPushButton {
            border: none;
            border-radius: 4px;
            color: white;
        }
        QPushButton:hover {
            background: rgba(0, 0, 0, 100);
        }
        QPushButton:pressed {
            background: rgba(255, 255, 255, 50);
        }
    )");


    createButtons();
    setFixedHeight(50);

}

void Tools::setPixmap(const QPixmap pix)
{
    raise();
    if(!pix.isNull()){
        this->_pixmap = pix;
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
    _clipboard->setPixmap(_pixmap);
    hide();
    emit closeWindow();
}

void Tools::createButtons()
{
    _buttonContainer = new QWidget(this);
    _buttonContainer->setStyleSheet("background: transparent;");

    QHBoxLayout *_layout = new QHBoxLayout(_buttonContainer);
    _layout->setSpacing(5);
    _layout->setContentsMargins(10,5,10,5);

    QPushButton*saveButton = new QPushButton(_buttonContainer);
    saveButton->setIcon(QIcon("/home/vivek/Codes/Qt/ScreenShot2/resources/save.png"));
    saveButton->setIconSize(QSize(24, 24));
    QPushButton*finishButton = new QPushButton(_buttonContainer);
    finishButton->setIcon(QIcon("/home/vivek/Codes/Qt/ScreenShot2/resources/yes.png"));
    finishButton->setIconSize(QSize(24, 24));
    QPushButton*closeButton = new QPushButton(_buttonContainer);
    closeButton->setIcon(QIcon("/home/vivek/Codes/Qt/ScreenShot2/resources/no.png"));
    closeButton->setIconSize(QSize(24, 24));

    connect(saveButton, &QPushButton::clicked, this, &Tools::onSaveClicked);
    connect(finishButton, &QPushButton::clicked, this, &Tools::onFinishClicked);
    connect(closeButton, &QPushButton::clicked, this, &Tools::onCloseClicked);


    _layout->addWidget(saveButton);
    _layout->addWidget(closeButton);
    _layout->addWidget(finishButton);

    _buttonContainer->setLayout(_layout);
}

