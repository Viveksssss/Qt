#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

#include "../global/Singleton.h"
#include "MysqlDao.h"
#include <mysql/mysql.h>

struct UserInfo;
class MysqlManager : public Singleton<MysqlManager> {
    friend class Singleton<MysqlManager>;

public:
    ~MysqlManager();

    int RegisterUser(const std::string& name, const std::string& email, const std::string& password);
    // bool CheckEmail(const std::string& name, const std::string& email);
    // bool UpdatePassword(const std::string& name, const std::string& email);
    // bool CheckPassword(const std::string& email, const std::string& pwd, UserInfo& userInfo);
    // bool TestProcedure(const std::string& name, const std::string& email);

private:
    MysqlManager();

private:
    MysqlDao _dao;
};

#endif // MYSQLMANAGER_H