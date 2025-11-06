#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <memory>
#include <QPixmap>
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
    // self
    void SetName(const QString&)noexcept;
    void SetToken(const QString&)noexcept;
    void SetUid(int)noexcept;
    void SetEmail(const QString&)noexcept;
    void SetAvatar(const QPixmap&)noexcept;

    int GetUid()noexcept;
    QString GetName()noexcept;
    QString GetToken()noexcept;
    QString GetEmail()noexcept;
    QPixmap GetAvatar()noexcept;

    // peer
    void SetPeerName(const QString&)noexcept;
    void SetPeerToken(const QString&)noexcept;
    void SetPeerUid(int)noexcept;
    void SetPeerEmail(const QString&)noexcept;
    void SetPeerAvatar(const QPixmap&)noexcept;

    int GetPeerUid()noexcept;
    QString GetPeerName()noexcept;
    QString GetPeerToken()noexcept;
    QString GetPeerEmail()noexcept;
    QPixmap GetPeerAvatar()noexcept;

private:
    UserManager();

private:
    QString _token;
    QString _name;
    QString _email;
    int _uid;
    QPixmap _avatar;

    QString _peer_token;
    QString _peer_name;
    QString _peer_email;
    int _peer_uid;
    QPixmap _peer_avatar;
};

#endif // USERMANAGER_H
