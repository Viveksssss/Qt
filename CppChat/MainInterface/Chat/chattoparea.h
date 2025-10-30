#ifndef CHATTOPAREA_H
#define CHATTOPAREA_H

#include <QWidget>
#include <QLabel>


class QPushButton;
class StatusLabel;
class ChatTopArea : public QWidget
{
    Q_OBJECT
public:
    explicit ChatTopArea(QWidget *parent = nullptr);
    void setupUI();
    void setupConnections();
signals:
private:
    // 变量声明顺序和ui顺序相同
    QLabel *statusLabel;
    QPushButton *addFriendBtn;
    QPushButton *newsBtn;
    QLabel *headerLabelFromChat;
    QPushButton *foldBtn;
};



class StatusLabel: public QLabel
{
    Q_OBJECT
public:
    explicit StatusLabel(QWidget *parent = nullptr);

    void setStatus(const QString &status);
    void setDotColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString status;
    QColor dotColor;

}

#endif // CHATTOPAREA_H
