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

UserManager::UserManager()
    : _name("")
    , _token("")
    , _uid(0)
{}
