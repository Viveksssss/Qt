#include "notificationpanel.h"
#include <QLabel>
#include <QVBoxLayout>

NotificationPanel::NotificationPanel(QWidget *parent)
    :QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();
    setupConnections();
}

void NotificationPanel::setupUI()
{
    setStyleSheet(R"(
        NotificationPanel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #FFFFFF, stop:1 #F8F9FA);
            border-left: 1px solid #E0E0E0;
            border-radius: 10px 0 0 10px;
        }
        QLabel {
            color: #333333;
            font-weight: bold;
            font-size: 14px;
            padding: 10px 15px;
            background-color: transparent;
        }
        QListWidget {
            background-color: transparent;
            border: none;
            outline: none;
        }
        QListWidget::item {
            padding: 12px 15px;
            border-bottom: 1px solid #F0F0F0;
            background-color: transparent;
        }
        QListWidget::item:hover {
            background-color: #F5F5F5;
        }
    )");

    QVBoxLayout *main_vlay = new QVBoxLayout(this);
    main_vlay->setContentsMargins(0,0,0,0);
    main_vlay->setSpacing(0);

    QLabel *title = new QLabel;
    title->setText("通知");
    QPalette palette = title->palette();
    palette.setColor(QPalette::WindowText,Qt::black);
    title->setPalette(palette);
    QFont font = title->font();
    font.setPointSize(20);
    title->setFont(font);

}

void NotificationPanel::setupConnections()
{

}
