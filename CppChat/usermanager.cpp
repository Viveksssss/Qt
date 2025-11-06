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

UserManager::UserManager()
    : _name("")
    , _token("")
    , _uid(0)
{}
