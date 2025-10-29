#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <memory>
#include "Properties/singleton.h"

class UserManager
    : public QObject
    , public Singleton<UserManager>
    , public std::enable_shared_from_this<UserManager>
{
    Q_OBJECT
    friend class Singleton<UserManager>;
public:
    ~UserManager() = default;
    void SetName(const QString&)noexcept;
    void SetToken(const QString&)noexcept;
    void SetUid(int)noexcept;
    void SetEmail(const QString&)noexcept;
    int GetUid()noexcept;
    QString GetName()noexcept;
    QString GetToken()noexcept;
    QString GetEmail()noexcept;

private:
    UserManager();

private:
    QString _token;
    QString _name;
    QString _email;
    int _uid;
};

#endif // USERMANAGER_H
