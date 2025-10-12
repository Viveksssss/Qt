#ifndef LOGINSCREEN_H
#define LOGINSCREEN_H

#include <QObject>
#include <QWidget>


class QComboBox;
class QLineEdit;
class QCheckBox;
class QLabel;
class QPushButton;
class QStackedWidget;
class RegisterScreen;
class LoginScreen;
class ForgotScreen;

class AuthStack:public QWidget
{
    Q_OBJECT
public:
    explicit AuthStack(QWidget *parent = nullptr);
private:
    QStackedWidget*stackWidget;
    LoginScreen*loginScreen;
    RegisterScreen*registerScreen;
    ForgotScreen*forgotScreen;

    QLabel *closeBtn;
signals:
    void closeWindow();
    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};



class LoginScreen : public QWidget
{
    Q_OBJECT
public:
    explicit LoginScreen(QWidget *parent = nullptr);

signals:
    void goRegsiter();
    void goForgotPassword();

private:
    void setupUI();
    void setupStyles();
    void setupConnections();

private slots:
    // void onLoginClicked();
    // void onRememberStateChanged(int state);
    // void onAutoLoginStateChanged(int state);


private:
    QLabel*headerLabel;

    // 账户密码
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;

    // 同意协议
    QCheckBox *agreeCheck;

    // 记住密码,自动登陆
    QCheckBox *rememberPasswd;
    QCheckBox *autoLoginCheck;

    // 登陆按钮
    QPushButton *loginBtn;

    // 注册,忘记密码
    QPushButton *registerBtn;
    QPushButton *forgotLabel;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *event);
};

#endif // LOGINSCREEN_H
