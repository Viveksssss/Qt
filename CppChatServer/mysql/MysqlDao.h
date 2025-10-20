#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mysql/mysql.h>
#include <queue>
#include <string>
#include <thread>

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

class MysqlPool {
public:
    struct conn_deleter {
        void operator()(MYSQL* conn) const noexcept
        {
            mysql_close(conn);
            mysql_thread_end();
        }
    };
    MysqlPool(const std::string& url, const std::string& user, const std::string& password, const std::string& schedma, int poolSize = std::thread::hardware_concurrency());

    std::unique_ptr<MYSQL, MysqlPool::conn_deleter> GetConnection() noexcept;
    void ReturnConnection(std::unique_ptr<MYSQL, conn_deleter> conn) noexcept;
    void Close() noexcept;
    ~MysqlPool();

private:
private:
    std::string _url;
    std::string _user;
    std::string _password;
    std::string _schedma;
    std::size_t _poolSize;
    std::queue<std::unique_ptr<MYSQL, conn_deleter>> _connections;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _stop;
};

class MysqlDao {
public:
    MysqlDao();
    ~MysqlDao();
    int RegisterUser(const std::string& name, const std::string& email, const std::string& password);

private:
    std::unique_ptr<MysqlPool> _pool;
};

#endif