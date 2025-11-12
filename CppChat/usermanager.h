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
    void SetSex(int sex)noexcept;
    void SetDesc(const QString&)noexcept;

    template <typename T>
    void SetIcon(T &&icon) noexcept
    {
        using Decayed = std::decay_t<T>;
        static_assert(std::is_same_v<Decayed, QString>,"SetIcon() only accepts QString (or QString&& / const QString&)");
        _icon = std::forward<T>(icon);   // 右值→移动，左值→拷贝
    }

    int GetUid()noexcept;
    QString GetName()noexcept;
    QString GetToken()noexcept;
    QString GetEmail()noexcept;
    QPixmap GetAvatar()noexcept;
    int GetSex()noexcept;
    QString GetDesc()noexcept;
    QString GetIcon()noexcept;

    // peer
    void SetPeerName(const QString&)noexcept;
    void SetPeerToken(const QString&)noexcept;
    void SetPeerUid(int)noexcept;
    void SetPeerEmail(const QString&)noexcept;
    void SetPeerAvatar(const QPixmap&)noexcept;
    void SetPeerSex(int sex)noexcept;
    void SetPeerDesc(const QString&)noexcept;

    template <typename T>
    void SetPerrIcon(T &&icon) noexcept
    {
        using Decayed = std::decay_t<T>;
        static_assert(std::is_same_v<Decayed, QString>,"SetPeerIcon() only accepts QString (or QString&& / const QString&)");
        _icon = std::forward<T>(icon);   // 右值→移动，左值→拷贝
    }

    int GetPeerUid()noexcept;
    QString GetPeerName()noexcept;
    QString GetPeerToken()noexcept;
    QString GetPeerEmail()noexcept;
    QPixmap GetPeerAvatar()noexcept;
    int GetPeerSex()noexcept;
    QString GetPeerDesc()noexcept;
    QString GetPeerIcon()noexcept;

private:
    UserManager();

private:
    QString _token;
    QString _name;
    QString _email;
    int _uid;
    QPixmap _avatar;
    int _sex;
    QString _desc;
    QString _icon;

    QString _peer_token;
    QString _peer_name;
    QString _peer_email;
    int _peer_uid;
    QPixmap _peer_avatar;
    int _peer_sex;
    QString _peer_desc;
    QString _peer_icon;
};

#endif // USERMANAGER_H
