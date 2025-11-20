#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

#include "../global/Singleton.h"
#include "MysqlDao.h"
#include <mysql/mysql.h>

class MysqlManager : public Singleton<MysqlManager> {
    friend class Singleton<MysqlManager>;

public:
    ~MysqlManager();
    /**
     * @brief 测试
     *
     * @param uid
     * @param email
     * @return int
     */
    int TestUidAndEmail(const std::string& uid, const std::string& email);
    /**
     * @brief 注册用户
     *
     * @param name
     * @param email
     * @param password
     * @return int
     */
    int RegisterUser(const std::string& name, const std::string& email, const std::string& password);
    /**
     * @brief 重置密码
     *
     * @param email
     * @param password
     * @return int
     */
    int ResetPassword(const std::string& email, const std::string& password);
    /**
     * @brief 检查密码
     *
     * @param user
     * @param password
     * @param userInfo
     * @return true
     * @return false
     */
    bool CheckPwd(const std::string& user, const std::string& password, UserInfo& userInfo);
    /**
     * @brief 发送请求前先检查是否也已经发送了好友请求，如果是，则直接成为好友，并且通知双方。
     *
     * @param fromUid
     * @param toUid
     * @return true
     * @return false
     */
    bool CheckApplied(const std::string& fromUid, const std::string& toUid);
    /**
     * @brief 添加好友申请
     *
     * @param fromUid
     * @param toUid
     * @return true
     * @return false
     */
    bool AddFriendApply(const std::string& fromUid, const std::string& toUid);
    /**
     * @brief 获取用户信息，精确匹配
     *
     * @param uid
     * @return std::shared_ptr<UserInfo>
     */
    std::shared_ptr<UserInfo> GetUser(int uid);
    /**
     * @brief 获取用户信息，模糊查询
     *
     * @return std::vector<std::shared_ptr<UserInfo>>
     */
    std::vector<std::shared_ptr<UserInfo>> GetUser(const std::string&);
    /**
     * @brief 获取好友申请列表
     *
     * @param uid
     * @param applyList
     * @return true
     * @return false
     */
    bool GetFriendApplyList(const std::string& uid, std::vector<std::shared_ptr<UserInfo>>& applyList);
    /**
     * @brief 改变好友申请状态,1同意0拒绝
     *
     * @param fromUid
     * @param toUid
     * @param status
     * @return true
     * @return false
     */
    bool ChangeApplyStatus(const std::string& fromUid, const std::string& toUid, int status);
    /**
     * @brief 改变消息状态,1已读0未读
     *
     * @param fromUid
     * @param toUid
     * @param status
     * @return true
     * @return false
     */
    bool ChangeMessageStatus(const std::string& uid, int status);
    /**
     * @brief 建立好友关系
     *
     * @param fromUid
     * @param toUid
     * @return true
     * @return false
     */
    bool MakeFriends(const std::string& fromUid, const std::string& toUid);
    /**
     * @brief 检查是否是好友关系
     *
     * @param fromUid
     * @param toUid
     * @return true
     * @return false
     */
    bool CheckIsFriend(int fromUid, int toUid);
    /**
     * @brief 添加通知
     *
     * @param uid
     * @param type
     * @param message
     * @return true
     * @return false
     */
    bool AddNotification(const std::string& uid, int type, const std::string& message);
    /**
     * @brief 获取通知列表
     *
     * @param uid
     * @param notificationList
     * @return true
     * @return false
     */
    bool GetNotificationList(const std::string& uid, std::vector<std::shared_ptr<UserInfo>>& notificationList);
    /**
     * @brief 返回好友列表
     *
     * @param uid
     * @return true
     * @return false
     */
    bool GetFriendList(const std::string& uid, std::vector<std::shared_ptr<UserInfo>>&, [[deprecated("暂时没有分段获取")]] int size = 20);
    /**
     * @brief 返回消息列表
     *
     * @param uid
     * @param size
     * @return true
     * @return false
     */
    bool GetMessageList(const std::string& uid, std::vector<std::shared_ptr<UserInfo>>&, [[deprecated("暂时没有分段获取")]] int size = 20);

private:
    MysqlManager();

private:
    MysqlDao _dao;
};

#endif // MYSQLMANAGER_H
