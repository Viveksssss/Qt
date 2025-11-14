#include "MysqlManager.h"
#include "MysqlDao.h"
MysqlManager::~MysqlManager()
{
}

int MysqlManager::TestUidAndEmail(const std::string& uid, const std::string& email)
{
    return _dao.TestUidAndEmail(uid, email);
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

bool MysqlManager::CheckPwd(const std::string& user, const std::string& password, UserInfo& userInfo)
{
    return _dao.CheckPwd(user, password, userInfo);
}

bool MysqlManager::AddFriendApply(const std::string& fromUid, const std::string& toUid)
{
    return _dao.AddFriendApply(fromUid, toUid);
}

std::shared_ptr<UserInfo> MysqlManager::GetUser(int uid)
{
    return _dao.GetUser(uid);
}
std::vector<std::shared_ptr<UserInfo>> MysqlManager::GetUser(const std::string& name)
{
    return _dao.GetUser(name);
}

bool MysqlManager::GetFriendApplyList(const std::string& uid, std::vector<std::shared_ptr<UserInfo>>& applyList)
{
    return _dao.GetFriendApplyList(uid, applyList);
}

bool MysqlManager::CheckApplied(const std::string& fromUid, const std::string& toUid)
{
    return _dao.CheckApplied(fromUid, toUid);
}

bool MysqlManager::ChangeApplyStatus(const std::string& fromUid, const std::string& toUid, int status)
{
    return _dao.ChangeApplyStatus(fromUid, toUid, status);
}

bool MysqlManager::MakeFriends(const std::string& fromUid, const std::string& toUid)
{
    return _dao.MakeFriends(fromUid, toUid);
}

bool MysqlManager::CheckIsFriend(const std::string& fromUid, const std::string& toUid)
{
    return _dao.CheckIsFriend(fromUid, toUid);
}