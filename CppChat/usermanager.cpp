#include "usermanager.h"


void UserManager::SetName(const QString &name)noexcept
{
    this->_name = name;
}

void UserManager::SetToken(const QString &token)noexcept
{
    this->_token = token;
}

void UserManager::SetUid(int uid) noexcept
{
    this->_uid = uid;
}

void UserManager::SetEmail(const QString &email) noexcept
{
    this->_email = email;
}

void UserManager::SetAvatar(const QPixmap &avatar) noexcept
{
    this->_avatar = avatar;
}

void UserManager::SetSex(int sex) noexcept
{
    this->_sex = sex;
}

void UserManager::SetDesc(const QString &desc) noexcept
{
    this->_desc = desc;
}


int UserManager::GetUid() noexcept
{
    return this->_uid;
}

QString UserManager::GetName() noexcept
{
    return this->_name;
}

QString UserManager::GetToken() noexcept
{
    return this->_token;
}

QString UserManager::GetEmail() noexcept
{
    return this->_email;
}

QPixmap UserManager::GetAvatar() noexcept
{
    return this->_avatar;
}

int UserManager::GetSex() noexcept
{
    return this->_sex;
}

QString UserManager::GetDesc() noexcept
{
    return this->_desc;
}

QString UserManager::GetIcon() noexcept
{
    return this->_icon;
}

void UserManager::SetPeerName(const QString &name) noexcept
{
    this->_peer_name = name;
}

void UserManager::SetPeerToken(const QString &token) noexcept
{
    this->_peer_token = token;
}

void UserManager::SetPeerUid(int uid) noexcept
{
    this->_peer_uid = uid;
}

void UserManager::SetPeerEmail(const QString &email) noexcept
{
    this->_peer_email = email;
}

void UserManager::SetPeerAvatar(const QPixmap &avatar) noexcept
{
    this->_peer_avatar = avatar;
}

void UserManager::SetPeerSex(int sex) noexcept
{
    this->_peer_sex = sex;
}

void UserManager::SetPeerDesc(const QString &desc) noexcept
{
    this->_peer_desc = desc;
}

int UserManager::GetPeerUid() noexcept
{
    return this->_peer_uid;
}

QString UserManager::GetPeerName() noexcept
{
    return this->_peer_name;
}

QString UserManager::GetPeerToken() noexcept
{
    return this->_peer_token;
}

QString UserManager::GetPeerEmail() noexcept
{
    return this->_peer_email;
}

QPixmap UserManager::GetPeerAvatar() noexcept
{
    return this->_peer_avatar;
}

int UserManager::GetPeerSex() noexcept
{
    return this->_peer_sex;
}

QString UserManager::GetPeerDesc() noexcept
{
    return this->_peer_desc;
}

QString UserManager::GetPeerIcon() noexcept
{
    return this->_peer_icon;
}

UserManager::UserManager()
    : _name("")
    , _token("")
    , _uid(0)
{}
