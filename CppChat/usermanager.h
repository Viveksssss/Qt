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
    void SetUid(int)noexcept;
    void SetStatus(int status = 1)noexcept;
    void SetSex(int sex)noexcept;
    void SetName(const QString&name = "卡皮巴拉")noexcept;
    void SetToken(const QString&)noexcept;
    void SetEmail(const QString&)noexcept;
    void SetAvatar(const QPixmap&)noexcept;
    void SetDesc(const QString&)noexcept;

    template <typename T>
    void SetIcon(T &&icon) noexcept
    {
        using Decayed = std::decay_t<T>;
        static_assert(std::is_same_v<Decayed, QString>,"SetIcon() only accepts QString (or QString&& / const QString&)");
        _icon = std::forward<T>(icon);   // 右值→移动，左值→拷贝
    }

    int GetUid()noexcept;
    int GetStatus()noexcept;
    int GetSex()noexcept;
    QString GetName()noexcept;
    QString GetToken()noexcept;
    QString GetEmail()noexcept;
    QPixmap GetAvatar()noexcept;
    QString GetDesc()noexcept;
    QString GetIcon()noexcept;

    // peer
    void SetPeerUid(int)noexcept;
    void SetPeerSex(int sex)noexcept;
    void SetPeerStatus(int status = 1)noexcept;
    void SetPeerName(const QString&)noexcept;
    void SetPeerToken(const QString&)noexcept;
    void SetPeerEmail(const QString&)noexcept;
    void SetPeerAvatar(const QPixmap&)noexcept;
    void SetPeerDesc(const QString&)noexcept;

    template <typename T>
    void SetPerrIcon(T &&icon) noexcept
    {
        using Decayed = std::decay_t<T>;
        static_assert(std::is_same_v<Decayed, QString>,"SetPeerIcon() only accepts QString (or QString&& / const QString&)");
        _icon = std::forward<T>(icon);   // 右值→移动，左值→拷贝
    }

    int GetPeerUid()noexcept;
    int GetPeerSex()noexcept;
    int GetPeerStatus()noexcept;
    QString GetPeerName()noexcept;
    QString GetPeerToken()noexcept;
    QString GetPeerEmail()noexcept;
    QPixmap GetPeerAvatar()noexcept;
    QString GetPeerDesc()noexcept;
    QString GetPeerIcon()noexcept;

private:
    UserManager();

private:
    QString _token;
    QString _name;
    QString _email;
    QPixmap _avatar;
    QString _desc;
    QString _icon;  // base64
    int _uid;
    int _sex;
    int _status;

    QString _peer_token;
    QString _peer_name;
    QString _peer_email;
    QPixmap _peer_avatar;
    QString _peer_desc;
    QString _peer_icon;
    int _peer_uid;
    int _peer_sex;
    int _peer_status;
};

#endif // USERMANAGER_H
