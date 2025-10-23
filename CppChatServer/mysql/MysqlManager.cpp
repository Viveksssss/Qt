#include "MysqlManager.h"
MysqlManager::~MysqlManager()
{
}

int MysqlManager::TestEmail(const std::string& email)
{
    return _dao.TestEmail(email);
}

MysqlManager::MysqlManager()
{
}

int MysqlManager::RegisterUser(const std::string& name, const std::string& email, const std::string& password)
{
    return _dao.RegisterUser(name, email, password);
}

int MysqlManager::ResetPassword(const std::string& email, const std::string& password)
{
    return _dao.ResetPassword(email, password);
}

// bool MysqlManager::CheckEmail(const std::string& name, const std::string& email)
// {
// }

// bool MysqlManager::UpdatePassword(const std::string& name, const std::string& email)
// {
// }

// bool MysqlManager::CheckPassword(const std::string& email, const std::string& pwd, UserInfo& userInfo)
// {
// }

// bool MysqlManager::TestProcedure(const std::string& name, const std::string& email)
// {
// }
