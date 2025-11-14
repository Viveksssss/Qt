#include "MysqlDao.h"
#include "../data/UserInfo.h"
#include "../global/ConfigManager.h"
#include "../global/const.h"
#include <exception>
#include <mysql++/connection.h>
#include <mysql++/exceptions.h>
#include <mysql++/result.h>
#include <mysql.h>
#include <spdlog/spdlog.h>

MysqlPool::MysqlPool(const std::string& url, const std::string& user, const std::string& password, const std::string& schedma, const std::string& port, int poolSize)
    : _url(url)
    , _user(user)
    , _password(password)
    , _schedma(schedma)
    , _port(port)
    , _poolSize(poolSize)
    , _stop(false)
{
    for (std::size_t i = 0; i < _poolSize; ++i) {
        try {
            auto conn = std::make_unique<mysqlpp::Connection>();
            if (conn->connect(_schedma.c_str(), _url.c_str(), _user.c_str(), _password.c_str(), std::stoi(_port))) {
                _connections.push(std::move(conn));
            } else {
                SPDLOG_ERROR("Failed To Create Database Connection: {}", conn->error());
            }
        } catch (const mysqlpp::Exception& e) {
            SPDLOG_ERROR("Failed to connect to mysql:{}", e.what());
        }
    }

    if (_connections.size() < _poolSize) {
        SPDLOG_WARN("Connection Pool Initialized With Only {}/{} Connections",
            _connections.size(), _poolSize);
    } else {
        SPDLOG_INFO("Mysql Connection Pool Initialized");
    }
}

MysqlPool::~MysqlPool()
{
    Close();
}

std::unique_ptr<mysqlpp::Connection> MysqlPool::GetConnection() noexcept
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this] {
        return _stop || !_connections.empty();
    });
    if (_stop) {
        return nullptr;
    }
    auto conn = std::move(_connections.front());
    _connections.pop();
    return conn;
}

void MysqlPool::ReturnConnection(std::unique_ptr<mysqlpp::Connection> conn) noexcept
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (_stop) {
        return;
    }
    _connections.push(std::move(conn));
    _cv.notify_one();
}

void MysqlPool::Close() noexcept
{
    std::unique_lock<std::mutex> lock(_mutex);
    _stop = true;
    _cv.notify_all();
    while (!_connections.empty()) {
        auto p = std::move(_connections.front());
        _connections.pop();
    }
}

MysqlDao::MysqlDao()
{
    auto& cfgMgr = ConfigManager::GetInstance();
    const auto& host = cfgMgr["Mysql"]["host"];
    const auto& port = cfgMgr["Mysql"]["port"];
    const auto& schema = cfgMgr["Mysql"]["schema"];
    const auto& password = cfgMgr["Mysql"]["password"];
    const auto& user = cfgMgr["Mysql"]["user"];
    _pool = std::make_unique<MysqlPool>(host, user, password, schema, port);
}

MysqlDao::~MysqlDao()
{
    _pool->Close();
}

int MysqlDao::TestUidAndEmail(const std::string& uid, const std::string& email)
{
    auto conn = _pool->GetConnection();
    if (!conn) {
        return -1;
    }
    Defer defer([this, &conn] {
        _pool->ReturnConnection(std::move(conn));
    });

    try {
        mysqlpp::Query query = conn->query();
        query << "select * from user where uid = ? or email = ?";
        query.parse();

        mysqlpp::StoreQueryResult res = query.store(uid, email);

        std::size_t count = res.num_rows();
        if (count != 1) {
            return -1;
        }
        return 1;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("MySQL++ exception in TestUidAndEmail: {}", e.what());
        return -1;
    }
}

int MysqlDao::RegisterUser(const std::string& name, const std::string& email, const std::string& password)
{
    auto conn = _pool->GetConnection();
    if (!conn) {
        return -1;
    }
    Defer defer([&conn, this] {
        _pool->ReturnConnection(std::move(conn));
    });

    try {
        mysqlpp::Transaction trans(*conn);
        mysqlpp::Query query = conn->query();

        // 先检查是否用户已经存在
        query << "SELECT id FROM user WHERE name = ? OR email = ? FOR UPDATE";
        auto check_result = query.store(name, email);

        if (check_result && check_result.num_rows() > 0) {
            trans.rollback();
            return -1;
        }

        // 如果不存在就插入，注册成功
        query << "INSERT INTO user (name, email, password) VALUES (?, ?, ?)";
        auto insert_result = query.execute(name, email, password);

        if (insert_result) {
            int new_id = query.insert_id();
            trans.commit();
            return new_id;
        } else {
            trans.rollback();
            return -1;
        }

    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return -1;
    }
}

int MysqlDao::ResetPassword(const std::string& email, const std::string& password)
{
    auto conn = _pool->GetConnection();
    if (!conn) {
        return -1;
    }
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return -1;
    }

    try {
        // 使用 mysql++ 预处理语句
        mysqlpp::Query query = conn->query();
        query << "UPDATE user SET password = ? WHERE email = ?";

        // 执行预处理更新
        mysqlpp::SimpleResult res = query.execute(password, email);

        if (res) {
            int affected_rows = res.rows();
            if (affected_rows > 0) {
                SPDLOG_INFO("Password reset successfully for email: {}, affected rows: {}", email, affected_rows);
                return 1; // 成功重置密码
            } else {
                SPDLOG_WARN("No user found with email: {}", email);
                return 0; // 没有找到用户，返回0
            }
        } else {
            SPDLOG_ERROR("Failed to reset password: {}", query.error());
            return -1;
        }

    } catch (const mysqlpp::BadQuery& e) {
        SPDLOG_ERROR("Bad query in ResetPassword: {}", e.what());
        return -1;
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception in ResetPassword: {}", e.what());
        return -1;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in ResetPassword: {}", e.what());
        return -1;
    }
}

bool MysqlDao::CheckPwd(const std::string& user, const std::string& password, UserInfo& userInfo)
{
    // chatServer暂时用不到。
    auto conn = _pool->GetConnection();
    if (!conn) {
        return false;
    }

    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });

    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return false;
    }
    try {
        mysqlpp::Query query = conn->query();
        query << "Select * from user where (uid = %0q or email = %1q) and password = %2q";
        query.parse();
        mysqlpp::StoreQueryResult res = query.store(user, user, password);
        std::size_t count = res.num_rows();
        if (count != 1) {
            return false;
        }
        return true;
    } catch (const mysqlpp::BadQuery& e) {
        SPDLOG_ERROR("Bad query in CheckPwd: {}", e.what());
        return false;
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception in CheckPwd: {}", e.what());
        return false;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in CheckPwd: {}", e.what());
        return false;
    }
}

bool MysqlDao::AddFriendApply(const std::string& fromUid, const std::string& toUid)
{
    if (fromUid == toUid) {
        return false; // 不允许自己添加自己
    }
    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return false;
    }

    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });

    try {
        mysqlpp::Query query = conn->query();
        query << "INSERT IGNORE INTO friend_apply (from_uid,to_uid) VALUES(%0,%1) ";
        query.parse();

        mysqlpp::SimpleResult res = query.execute(std::stoi(fromUid), std::stoi(toUid));
        int rowCount = res.rows();
        return rowCount >= 0;
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        return false;
    }

    return true;
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return nullptr;
    }

    try {
        mysqlpp::Query query = conn->query();
        query << "SELECT name, email, password,status FROM user WHERE uid = %0q";

        query.parse();
        mysqlpp::StoreQueryResult res = query.store(uid);

        /**
                            jj["uid"] = uid;
                            jj["name"] = user_info->name;
                            jj["email"] = user_info->email;
                            jj["nick"] = user_info->nick;
                            jj["sex"] = user_info->sex;
                            jj["desc"] = user_info->desc;
                            jj["icon"] = user_info->icon;
                            jj["token"] = token;
         *
         */

        if (res && res.num_rows() == 1) {
            auto user_info = std::make_shared<UserInfo>();
            user_info->uid = uid;
            user_info->sex = res[0]["sex"];
            user_info->status = res[0]["status"];
            user_info->name = std::string(res[0]["name"]);
            user_info->email = std::string(res[0]["email"]);
            user_info->icon = std::string(res[0]["icon"]);
            user_info->desc = std::string(res[0]["desc"]);
            // user_info->nick = std::string(res[0]["nick"]);

            _pool->ReturnConnection(std::move(conn));
            return user_info;
        } else {
            _pool->ReturnConnection(std::move(conn));
            SPDLOG_DEBUG("User not found with uid: {}", uid);
            return nullptr;
        }

    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        if (conn)
            _pool->ReturnConnection(std::move(conn));
        return nullptr;
    }
}

std::vector<std::shared_ptr<UserInfo>> MysqlDao::GetUser(const std::string& name)
{
    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return {};
    }

    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });

    try {
        mysqlpp::Query query = conn->query();

        // 使用预处理语句进行模糊查询
        query << "SELECT * FROM user WHERE name LIKE " << mysqlpp::quote << ("%" + name + "%");

        mysqlpp::StoreQueryResult res = query.store();
        std::vector<std::shared_ptr<UserInfo>> users;

        if (res) {
            users.reserve(res.num_rows()); // 预分配内存
            for (size_t i = 0; i < res.num_rows(); ++i) {
                auto user_info = std::make_shared<UserInfo>();
                user_info->uid = res[i]["uid"];
                user_info->sex = res[i]["sex"];
                user_info->status = res[i]["status"];
                user_info->name = std::string(res[i]["name"]);
                user_info->email = std::string(res[i]["email"]);
                user_info->icon = std::string(res[0]["icon"]);
                user_info->desc = std::string(res[0]["desc"]);
                // user_info->nick = std::string(res[0]["nick"]);

                users.push_back(user_info);
            }
            SPDLOG_DEBUG("Found {} users matching pattern: '{}'", users.size(), search_pattern);
        }
        return users;
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        return {};
    }
}

bool MysqlDao::GetFriendApplyList(const std::string& uid, std::vector<std::shared_ptr<UserInfo>>& applyList)
{
    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return false;
    }

    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });

    try {
        mysqlpp::Query query = conn->query();
        query << "SELECT u.uid, u.name, u.icon, u.desc, u.sex, fa.time "
              << "FROM user u, friend_apply fa "
              << "WHERE u.uid = fa.from_uid " // 获取申请发送者的信息
              << "AND fa.to_uid = %0q " // 当前用户是接收方
              << "AND fa.status = 0 " // status = 0 表示等待处理
              << "ORDER BY fa.time desc";
        query.parse();
        mysqlpp::StoreQueryResult res = query.store(uid);

        if (res && res.num_rows() > 0) {
            applyList.reserve(res.num_rows()); // 预分配内存
            for (size_t i = 0; i < res.num_rows(); ++i) {
                auto user_info = std::make_shared<UserInfo>();
                user_info->uid = res[i]["uid"];
                user_info->sex = res[i]["sex"];
                user_info->name = std::string(res[i]["name"]);
                user_info->icon = std::string(res[i]["icon"]);
                user_info->desc = std::string(res[i]["desc"]);
                user_info->back = std::string(res[i]["time"]);
                applyList.push_back(user_info);
            }
            return true;
        }
        return false;
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        return false;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}

bool MysqlDao::CheckApplied(const std::string& fromUid, const std::string& toUid)
{
    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return false;
    }
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });

    try {
        mysqlpp::Query query = conn->query();
        query << "SELECT COUNT(*) as cnt from friend_apply where from_uid = %0q and to_uid = %1q";
        query.parse();
        mysqlpp::StoreQueryResult res = query.store(std::stoi(fromUid), std::stoi(toUid));
        if (res && res.num_rows() == 1) {
            int count = res[0]["cnt"];
            return count > 0;
        }
        return false;
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        return false;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}

bool MysqlDao::ChangeApplyStatus(const std::string& fromUid, const std::string& toUid, int status)
{
    if (!status || fromUid == toUid) {
        return false;
    }
    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return false;
    }
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });

    try {
        mysqlpp::Query query = conn->query();
        query << "UPDATE friend_apply SET status = %0q WHERE from_uid = %1q AND to_uid = %2q";
        query.parse();
        mysqlpp::SimpleResult res = query.execute(status, std::stoi(fromUid), std::stoi(toUid));
        if (res) {
            int affected_rows = res.rows();
            if (affected_rows > 0) {
                SPDLOG_INFO("Apply status changed successfully for from_uid: {}, to_uid: {}, status: {}", fromUid, toUid, status);
                return true;
            } else {
                SPDLOG_WARN("No apply found with from_uid: {}, to_uid: {}", fromUid, toUid);
                return false;
            }
        } else {
            SPDLOG_ERROR("Failed to change apply status: {}", query.error());
            return false;
        }

    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        return false;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}

bool MysqlDao::MakeFriends(const std::string& fromUid, const std::string& toUid)
{
    if (fromUid == toUid) {
        return false;
    }

    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return false;
    }
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });
    try {
        mysqlpp::Transaction trans(*conn);
        // 添加好友应该是双向的，所以需要插入两条记录
        mysqlpp::Query query1 = conn->query();
        mysqlpp::Query query2 = conn->query();
        query1 << "INSERT INTO friends (self_id,friend_id) VALUES(%0q,%1q)";
        query2 << "INSERT INTO friends (self_id,friend_id) VALUES(%1q,%0q)";
        query1.parse();
        query2.parse();
        mysqlpp::SimpleResult res1 = query1.execute(std::stoi(fromUid), std::stoi(toUid));
        mysqlpp::SimpleResult res2 = query2.execute(std::stoi(toUid), std::stoi(fromUid));
        if (res1 && res2) {
            int affected_rows1 = res1.rows();
            int affected_rows2 = res2.rows();
            if (affected_rows1 > 0 && affected_rows2 > 0) {
                trans.commit();
                SPDLOG_INFO("Friends added successfully for from_uid: {}, to_uid: {}", fromUid, toUid);
                return true;
            } else {
                trans.rollback();
                SPDLOG_WARN("Failed to add friends for from_uid: {}, to_uid: {}", fromUid, toUid);
                return false;
            }
        } else {
            trans.rollback();
            SPDLOG_ERROR("Failed to add friends for from_uid: {}, to_uid: {}", fromUid, toUid);
            return false;
        }
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        return false;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}

bool MysqlDao::CheckIsFriend(const std::string& fromUid, const std::string& toUid)
{
    if (fromUid == toUid) {
        return true;
    }
    auto conn = _pool->GetConnection();
    if (!conn) {
        SPDLOG_ERROR("Failed to get connection from pool");
        return false;
    }
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });
    try {
        mysqlpp::Query query = conn->query();
        query << "SELECT COUNT(*) as cnt from friends where (self_id = %0q and friend_id = %1q)"
              << "OR (self_id = %1q and friend_id = %0q)";
        query.parse();
        mysqlpp::StoreQueryResult res = query.store(std::stoi(fromUid), std::stoi(toUid));
        if (res && !res.empty()) {
            int count = res[0]["cnt"];
            return count == 2;
        }
        return false;
    } catch (const mysqlpp::Exception& e) {
        SPDLOG_ERROR("MySQL++ exception: {}", e.what());
        return false;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception: {}", e.what());
        return false;
    }
}