#ifndef REGISTERSCREEN_H
#define REGISTERSCREEN_H

#include <QWidget>
#include <QHash>
#include <functional>
#include "../Properties/global.h"
#include <QObject>


class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QFormLayout;
class RegisterScreen : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterScreen(QWidget *parent = nullptr);

signals:
    void goLogin();

private:
    void setupUI();
    void setupConnections();
    void showTip(int code,const QString &str="注册账号");
    int doVerify(bool includingSecurityCode=false);
    void initHttpHandlers();
private slots:
    void do_get_code_clicked();
    void do_get_code_finished(RequestType requestType,const QString&res,ErrorCodes errorCode);
    void do_change_get_code_btn();
private:

    QHash<RequestType,std::function<void(const QJsonObject&)>> _handlers;
    QTimer*timer;
    int countdown;

    QFormLayout *formLayout;

    //顶层
    QLabel*headerLabel;
    QLabel*registerTitle;

    // 账户密码
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QLineEdit *passwordSure;

    // 邮箱
    QLineEdit *emailEdit;

    // 获取验证码
    QPushButton *getSecurityCode;
    QLineEdit *securityCode;

    // 注册按钮,取消
    QPushButton *registerBtn;
    QPushButton *cancelBtn;


signals:
};

#endif // REGISTERSCREEN_H
