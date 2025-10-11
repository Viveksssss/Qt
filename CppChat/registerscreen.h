#ifndef REGISTERSCREEN_H
#define REGISTERSCREEN_H

#include <QWidget>


class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;
class RegisterScreen : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterScreen(QWidget *parent = nullptr);

signals:
    void goLogin();

private:
    void setupUI();
    void setupStyles();
    void setupConnections();

private:
    QLabel*headerLabel;

    // 账户密码
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QLineEdit *passwordSure;

    // 邮箱
    QLineEdit *mailEdit;

    // 获取验证码
    QPushButton *getSecurityCode;
    QLineEdit *securityCode;

    // 注册按钮,取消
    QPushButton *registerBtn;
    QPushButton *cancelBtn;


signals:
};

#endif // REGISTERSCREEN_H
