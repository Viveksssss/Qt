#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <queue>
#include <string>
#include <thread>

#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>
// #include <mysql/mysql.h> // 移除c的封装改用cpp

/**
 ┌─────────────────┐       ┌──────────────────┐       ┌─────────────────┐
 │   MysqlDao      │       │   MySqlPool      │       │ UserInfo (DTO)  │
 │                 │       │                  │       │                 │
 │ - pool_         │───────│ - connections    │       │ - name          │
 │                 │       │ - pool params    │       │ - email         │
 │ + RegUser()     │       │ + getConnection()│       │ - password      │
 │ + other CRUD    │       │ + returnConn()   │       │                 │
 └─────────────────┘       └──────────────────┘       └─────────────────┘
          │                           │
          │                           │
          ▼                           ▼
 ┌─────────────────┐       ┌──────────────────┐
 │  Business Logic │       │ mysql::Connection│
 │   (Service层)   │       │  (MySQL驱动)      │
 └─────────────────┘       └──────────────────┘
 *
 */

struct UserInfo;

class MysqlPool {
public:
    MysqlPool(const std::string& url, const std::string& user, const std::string& password, const std::string& schedma, const std::string& port = "3306", int poolSize = std::thread::hardware_concurrency());

    std::unique_ptr<mysqlpp::Connection> GetConnection() noexcept;
    void ReturnConnection(std::unique_ptr<mysqlpp::Connection> conn) noexcept;
    void Close() noexcept;
    ~MysqlPool();

private:
    std::string _schedma;
    std::string _user;
    std::string _password;
    std::string _url;
    std::string _port;
    std::size_t _poolSize;
    std::queue<std::unique_ptr<mysqlpp::Connection>> _connections;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _stop;
};

class MysqlDao {
public:
    MysqlDao();
    ~MysqlDao();
    int TestUidAndEmail(const std::string& uid, const std::string& email);
    int RegisterUser(const std::string& name, const std::string& email, const std::string& password);
    int ResetPassword(const std::string& email, const std::string& password);
    bool CheckPwd(const std::string& user, const std::string& password, UserInfo& userInfo);
    bool AddFriendApply(const std::string&fromUid,const std::string&toUid);
    std::shared_ptr<UserInfo> GetUser(int uid);
    std::vector<std::shared_ptr<UserInfo>> GetUser(const std::string& name);

private:
    std::unique_ptr<MysqlPool> _pool;
};

#endif
