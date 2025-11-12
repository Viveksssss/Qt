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
class SideNews;
class ChatTopArea : public QWidget
{
    Q_OBJECT
public:
    explicit ChatTopArea(QWidget *parent = nullptr);
    ~ChatTopArea();
    void setupUI();
    void setupConnections();
signals:
    void on_search_friend(const QString&uid);
public slots:
    void do_show_news();

private:
    // 变量声明顺序和ui顺序相同
    StatusLabel *statusLabel;
    QPushButton *newsBtn;
    AnimatedSearchBox *searchBox;
    ClearAvatarLabel *headerLabelFromChat;
    QPushButton *foldBtn;
    FriendAddDialog *friendAddDialog;
    QList<std::shared_ptr<UserInfo>>userLists;
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
    void setEnabled(bool enabled = true);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString status;
    QColor dotColor;

    bool isHovered = false;
    bool isPressed = false;
    bool isEnabled = true;
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


class FriendsItem :public QWidget
{
    Q_OBJECT
public:
    explicit FriendsItem(int uid,const QString&avatar_path = "",const QString&name = "",const QString&status = "在线",QWidget*parent=nullptr);
    void setupUI();
    void setupConnections();
signals:
    void on_apply_clicked(int uid);
private:
    int _uid;
    QString _avatar_path;
    QString _name;
    QPushButton *_applyFriend;
    StatusLabel *_statusLabel;
    QLabel * _avatar;
    QString _status;
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
public slots:
    // 收缩查询框的按键
    void do_search_clcked();
    // 根据输入内容决定是否发送请求
    void do_text_changed(const QString &text);
    // 获取tcp回包，设置usersList
    void do_users_searched(QList<std::shared_ptr<UserInfo>>)noexcept;

private:
    // 发送tcp请求，查询用户
    void getSearchUsers(const QString &uid);
    int searchWidth() const { return textWidth; }
    void setSearchWidth(int width);
    void hideResults();
    void showResults();
    void addItemToResults();
    void updateResults();

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
    QList<std::shared_ptr<UserInfo>>usersList;

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
    void searchFriend(int uid);
    void setUserName(const QString&name);
    void setUserUid(int uid);
    void setUserAvatar(const QString&avatar);
    void setRemark(const QString&remark);

public slots:
    void do_add_friend(int uid);
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

    int userUid;
    QString userName;
    QString userAvatar;
};




#endif // CHATTOPAREA_H
