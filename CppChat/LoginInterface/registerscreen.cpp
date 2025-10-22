#include "registerscreen.h"
#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QRegularExpression>
#include <QTimer>
#include "../stylemanager.h"
#include "../httpmanager.h"

RegisterScreen::RegisterScreen(QWidget *parent)
    : QWidget{parent}
{
    initHttpHandlers();
    setupUI();
    setupConnections();

    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
    setFocusPolicy(Qt::ClickFocus);   // 允许鼠标点击时自己拿焦点

    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.width() - width()/2,screenGeometry.height() - height()/2);
}

void RegisterScreen::setupUI()
{
    this->setObjectName("registerScreen");
    this->setAttribute(Qt::WA_StyledBackground);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30,30,30,30);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignTop);

    // 表单布局
    formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignHCenter);

    // 头像
    headerLabel = new QLabel;
    headerLabel->setObjectName("headerLabel");
    headerLabel->setFixedSize(80,80);
    headerLabel->setAlignment(Qt::AlignHCenter);
    headerLabel->setPixmap(QPixmap(":/Resources/QQ.png"));
    headerLabel->setScaledContents(true);

    registerTitle = new QLabel;
    registerTitle->setObjectName("registerTitle");
    registerTitle->setText("注册账号");
    registerTitle->setProperty("state","normal");

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addStretch();
    headerLayout->addWidget(headerLabel);
    headerLayout->addWidget(registerTitle);
    headerLayout->addStretch();

    // 账号
    accountEdit = new QLineEdit();
    accountEdit->setObjectName("accountEdit");
    accountEdit->setFixedHeight(30);
    accountEdit->setAlignment(Qt::AlignHCenter);
    accountEdit->setPlaceholderText("输入用户名");
    accountEdit->clearFocus();
    formLayout->addRow("账号:",accountEdit);

    // 邮箱
    emailEdit = new QLineEdit();
    emailEdit->setObjectName("emailEdit");
    emailEdit->setFixedHeight(30);
    emailEdit->setAlignment(Qt::AlignHCenter);
    emailEdit->setPlaceholderText("请输入邮箱");
    emailEdit->clearFocus();
    formLayout->addRow("邮箱:",emailEdit);

    // 密码
    passwordEdit = new QLineEdit();
    passwordEdit->setObjectName("passwordEdit");
    passwordEdit->setFixedHeight(30);
    passwordEdit->setAlignment(Qt::AlignHCenter);
    passwordEdit->setPlaceholderText("输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setDragEnabled(false);
    formLayout->addRow("密码:",passwordEdit);

    // 确认密码
    passwordSure = new QLineEdit();
    passwordSure->setObjectName("passwordSure");
    passwordSure->setFixedHeight(30);
    passwordSure->setAlignment(Qt::AlignHCenter);
    passwordSure->setPlaceholderText("确认输入QuickChat密码");
    passwordSure->setEchoMode(QLineEdit::Password);
    passwordSure->setDragEnabled(false);
    formLayout->addRow("确认密码:",passwordSure);

    // 验证码
    securityCode = new QLineEdit();
    securityCode->setObjectName("securityCode");
    securityCode->setFixedHeight(30);
    securityCode->setAlignment(Qt::AlignHCenter);
    securityCode->setPlaceholderText("输入验证码");

    getSecurityCode = new QPushButton();
    getSecurityCode->setObjectName("getSecurityCode");
    getSecurityCode->setText("获取");

    QHBoxLayout *securityCodeLayout = new QHBoxLayout;
    securityCodeLayout->addWidget(securityCode);
    securityCodeLayout->addWidget(getSecurityCode);
    formLayout->addRow("获取验证码:",securityCodeLayout);

    // 注册,取消
    registerBtn = new QPushButton();
    registerBtn->setObjectName("registerBtn2");
    registerBtn->setText("注册");
    registerBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    registerBtn->setMinimumWidth(60);
    registerBtn->setMinimumHeight(30);

    cancelBtn = new QPushButton();
    cancelBtn->setObjectName("cancelBtn");
    cancelBtn->setText("取消");
    cancelBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    cancelBtn->setMinimumWidth(60);
    cancelBtn->setMinimumHeight(30);

    QHBoxLayout *registerAndCancelLayout = new QHBoxLayout;
    registerAndCancelLayout->addWidget(registerBtn);
    registerAndCancelLayout->addWidget(cancelBtn);

    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(registerAndCancelLayout);
}


void RegisterScreen::setupConnections()
{
    // 取消返回登陆节目
    connect(cancelBtn,&QPushButton::clicked,this,[=](){emit goLogin();});
    // 点击获取验证码按钮
    connect(getSecurityCode,&QPushButton::clicked,this,&RegisterScreen::do_get_code_clicked);
    // 点击注册按钮
    connect(registerBtn,&QPushButton::clicked,this,&RegisterScreen::do_register_clicked);

    // 验证码
    connect(HttpManager::GetInstance().get(),&HttpManager::on_get_code_finished,this,&RegisterScreen::do_get_code_finished);
    // 注册
    connect(HttpManager::GetInstance().get(),&HttpManager::on_register_finished,this,&RegisterScreen::do_register_finished);
}

void RegisterScreen::showTip(int code,const QString&str)
{
    if(!code){
        registerTitle->setText(str);
        registerTitle->setProperty("state","error");
        StyleManager::repolish(registerTitle);
    }else{
        registerTitle->setText(str);
        registerTitle->setProperty("state","normal");
        StyleManager::repolish(registerTitle);
    }
}

bool RegisterScreen::doVerify(bool includingSecurityCode)
{
    // 是否未填写
    bool allFilled = true;
    // 是否包括验证码?分别用户获取验证码和登陆两个按钮的逻辑
    std::size_t count= formLayout->rowCount() -(includingSecurityCode?0:1);
    for(int i = 0;i<count;++i){
        QLayoutItem*item = formLayout->itemAt(i,QFormLayout::FieldRole);
        if(item&&item->widget()){
            QLineEdit*lineEdit = static_cast<QLineEdit*>(item->widget());
            if(lineEdit && lineEdit->text().trimmed().isEmpty()){
                lineEdit->setToolTip("此项不能为空");
                lineEdit->setStyleSheet("border: 1px solid red;");
                allFilled = false;
            }
            else{
                lineEdit->setToolTip("");
                lineEdit->setStyleSheet("");
            }
        }
    }


    // 邮箱
    auto email = emailEdit->text();
    QRegularExpression reg(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    bool matched = reg.match(email).hasMatch();

    RegisterVarify rv;
    rv = RegisterVarify::SUCCESS;
    if (!matched){
        rv = RegisterVarify::EMAIL_INCORRECTFORMAT;
    }else if(passwordEdit->text().trimmed()!=passwordSure->text().trimmed()){
        rv = RegisterVarify::PASSWORD_NOTSURE;
    }else if(!allFilled){
        rv = RegisterVarify::CONTENT_INCOMPLETE;
    }

    bool ok = false;
    switch(rv)
    {
    case RegisterVarify::CONTENT_INCOMPLETE:
        showTip(0,"请填写完整内容");
        break;
    case RegisterVarify::EMAIL_INCORRECTFORMAT:
        showTip(0,"邮箱格式不正确");
        break;
    case RegisterVarify::PASSWORD_NOTSURE:
        showTip(0,"两次输入密码不同");
        passwordEdit->setStyleSheet("border: 1px solid red;");
        passwordEdit->setToolTip("此项不能为空");
        passwordSure->setStyleSheet("border: 1px solid red;");
        passwordSure->setToolTip("此项不能为空");
        break;
    case RegisterVarify::SUCCESS:
        showTip(1,"请注意查收");
        ok = true;
        break;
    }
    return ok;
}

void RegisterScreen::initHttpHandlers()
{
    // 注册
    _handlers[RequestType::GET_SECURITY_CODE] = [this](const QJsonObject&obj){
        ErrorCodes error = static_cast<ErrorCodes>(obj["error"].toInt());
        if(error!=ErrorCodes::SUCCESS){
            showTip(3,"注册失败");
            return;
        }
        auto success = obj["success"].toBool();
        auto message = obj["message"].toString();
        qDebug() << "success:" << success;
        qDebug() << "message:" << message ;
    };
    _handlers[RequestType::REG_USER] = [this](const QJsonObject&obj){
        int error = obj["error"].toInt();
        if(error !=static_cast<int>(ErrorCodes::SUCCESS)){
            showTip(0,tr("账号或邮箱已被占用"));
            return;
        }
        auto email = obj["email"].toString();
        showTip(1,tr("用户注册成功"));
        qDebug()<< "Email is " << email ;
    };
}

void RegisterScreen::do_get_code_clicked()
{
    auto res= doVerify();
    if (!res) return;

    QJsonObject json_obj;
    json_obj["email"] = emailEdit->text().trimmed();
    HttpManager::GetInstance()->PostHttp(QUrl(gate_url_prefix+"/getSecurityCode"),json_obj,RequestType::GET_SECURITY_CODE,Modules::REGISTERMOD);

    timer = new QTimer(this);
    countdown = 60;

    getSecurityCode->setEnabled(false);
    getSecurityCode->setText(QString("%1s").arg(countdown));

    // 获取按钮的文字变化
    QObject::connect(timer,&QTimer::timeout,this,&RegisterScreen::do_change_get_code_btn);
    timer->start(1000);
    // 60s之后恢复
    QTimer::singleShot(60000,this,[this](){
        if (timer) {
            timer->stop();
            timer->deleteLater();
            timer = nullptr;
        }
        getSecurityCode->setText("获取");
        getSecurityCode->setEnabled(true);
    });
}

void RegisterScreen::do_get_code_finished(RequestType requestType,const QString&res,ErrorCodes errorCode)
{
    if(errorCode!=ErrorCodes::SUCCESS){
        showTip(0,tr("网络请求错误"));
        return;
    }
    // 解析json字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    qDebug() << jsonDoc.toJson();
    if(jsonDoc.isEmpty()){
        showTip(0,"接收数据异常为空,无法解析");
        return;
    }
    if(!jsonDoc.isObject()){
        showTip(0,"接收数据异常,无法解析");
        return;
    }
    // 调用对应的回调解析
    _handlers[requestType](jsonDoc.object());
    return;
}

void RegisterScreen::do_change_get_code_btn()
{
    if (countdown > 0) {
        countdown--;
        getSecurityCode->setText(QString("(%1s)").arg(countdown));
    }
}

void RegisterScreen::do_register_clicked()
{
    auto res= doVerify();
    if(!res){
        return;
    }
    if (securityCode->text().trimmed().isEmpty()){
        securityCode->setStyleSheet("border: 1px solid red;");
        securityCode->setToolTip("此项不能为空");
    }else{
        securityCode->setStyleSheet("");
    }

    QJsonObject j;
    j["user"] = accountEdit->text().trimmed();
    j["password"] = passwordEdit->text().trimmed();
    j["email"] = emailEdit->text().trimmed();
    j["securityCode"] = securityCode->text().trimmed();

    HttpManager::GetInstance()->PostHttp(QUrl(gate_url_prefix+"/userRegister"),
                                        j, RequestType::REG_USER,Modules::REGISTERMOD);

}

void RegisterScreen::do_register_finished(RequestType requestType,const QString&res,ErrorCodes errorCode)
{
    qDebug() << static_cast<int>(errorCode);
    qDebug() << static_cast<int>(ErrorCodes::SUCCESS);
    if(errorCode!=ErrorCodes::SUCCESS){
        showTip(0,tr("网络请求错误"));
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    qDebug() << jsonDoc.toJson();
    if(jsonDoc.isEmpty()){
        showTip(0,"接收数据异常为空,无法解析");
        return;
    }
    if(!jsonDoc.isObject()){
        showTip(0,"接收数据异常,无法解析");
        return;
    }

    _handlers[requestType](jsonDoc.object());
    return;
}
