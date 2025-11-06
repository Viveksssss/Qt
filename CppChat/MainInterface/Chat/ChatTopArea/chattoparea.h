#ifndef CHATTOPAREA_H
#define CHATTOPAREA_H

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPen>
#include <QPainterPath>
#include <QPainter>
#include "../../../Properties/global.h"

class QVBoxLayout;
class QTextEdit;
class QListWidgetItem;
class QToolButton;
class QHBoxLayout;
class QListWidget;
class QLineEdit;

class QPushButton;
class StatusLabel;
class StatusButton;
class AnimatedSearchBox;
class ClearAvatarLabel;
class FriendAddDialog;
class ChatTopArea : public QWidget
{
    Q_OBJECT
public:
    explicit ChatTopArea(QWidget *parent = nullptr);
    void setupUI();
    void setupConnections();
signals:
    void on_add_friend(const QString&uid);
private:
    // 变量声明顺序和ui顺序相同
    StatusLabel *statusLabel;
    QPushButton *newsBtn;
    AnimatedSearchBox *searchBox;
    ClearAvatarLabel *headerLabelFromChat;
    QPushButton *foldBtn;
    FriendAddDialog *friendAddDialog;

    // QObject interface
public:
    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
};


class StatusLabel: public QLabel
{
    Q_OBJECT
public:
    explicit StatusLabel(QWidget *parent = nullptr);

    void setStatus(const QString &status);
    QString getStatus();
    void setDotColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString status;
    QColor dotColor;

    bool isHovered = false;
    bool isPressed = false;
signals:
    void clicked();
    void hover();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event)override;
    void enterEvent(QEnterEvent *event)override;
    void leaveEvent(QEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
};



class AnimatedSearchBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int searchWidth READ searchWidth WRITE setSearchWidth)

public:
    explicit AnimatedSearchBox(QWidget *parent = nullptr);
    void toggleSearch();  // 切换搜索框显示/隐藏
    QString getContent();

signals:
    void on_search_clicked(const QString &keyword);
    void on_item_clicked(QListWidgetItem*item);
private slots:
    void do_search_clcked();
    void do_text_changed(const QString &text);
    void do_item_clicked(QListWidgetItem*);

private:
    int searchWidth() const { return textWidth; }
    void setSearchWidth(int width);
    void hideResults();
    void showResults();
    void updateResults(const QString &keyword);

    void setupUI();
    void startAnimation();
    void setupConnections();

private:
    QHBoxLayout *layout;
    QLineEdit *searchEdit;
    QToolButton *searchButton;
    QPropertyAnimation *animation;
    QAction *clearAction;
    QGraphicsOpacityEffect *opacityEffect;
    QListWidget *resultList;

    int textWidth;
    bool isExpanded;
    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};



class ClearAvatarLabel : public QLabel {
public:
    explicit ClearAvatarLabel(QWidget* parent = nullptr) : QLabel(parent) {
        setFixedSize(30, 30);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        // 绘制头像
        if (!pixmap().isNull()) {
            QPixmap scaledPixmap = getHighQualityPixmap();

            // 圆形裁剪
            QPainterPath path;
            path.addEllipse(rect());
            painter.setClipPath(path);

            // 绘制图片
            painter.drawPixmap(rect(), scaledPixmap);
        }

        // 绘制边框
        painter.setClipping(false);
        painter.setPen(QPen(QColor("#3b3b3b"), 2));
        painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
    }

private:
    QPixmap getHighQualityPixmap() {
        QPixmap original = pixmap();
        if (original.isNull()) return original;

        // 高质量缩放
        return original.scaled(size() * 2, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            .scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
};




class FriendAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FriendAddDialog(QWidget *parent = nullptr);
    void searchFriend(const QString&uid);
    void setUserName(const QString&name);
    void setUserUid(const QString&uid);
    void setUserAvatar(const QString&avatar);
    void setRemark(const QString&remark);

public slots:
    void do_add_friend(const QString&uid);
private:
    void setupUI();

    // UI components
    QLabel *titleLabel;
    QLabel *avatarLabel;
    QLabel *nameLabel;
    QLabel *uidLabel;
    QPushButton *addButton;
    QPushButton *cancelButton;
    QTextEdit *remarkEdit;

    QString userUid;
    QString userName;
    QString userAvatar;
};




#endif // CHATTOPAREA_H
