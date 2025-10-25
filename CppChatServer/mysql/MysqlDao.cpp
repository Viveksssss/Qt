#include "MysqlDao.h"
#include "../global/ConfigManager.h"
#include "../global/const.h"
#include <spdlog/spdlog.h>
MysqlPool::MysqlPool(const std::string& url, const std::string& user, const std::string& password, const std::string& schedma, int poolSize)
    : _url(url)
    , _user(user)
    , _password(password)
    , _schedma(schedma)
    , _poolSize(poolSize)
    , _stop(false)
{
    int successCount = 0;
    try {
        for (std::size_t i = 0; i < _poolSize; ++i) {
            MYSQL* conn = mysql_init(nullptr);
            if (conn == nullptr) {
                throw std::runtime_error("mysql_init failed");
            }

            if (mysql_real_connect(conn, _url.c_str(), _user.c_str(), _password.c_str(), _schedma.c_str(), 0, NULL, 0) == nullptr) {
                throw std::runtime_error("mysql_real_connect failed");
            }

            if (mysql_ping(conn) != 0) {
                std::string error = "mysql_ping failed: ";
                error += mysql_error(conn);
                mysql_close(conn);
                throw std::runtime_error(error);
            }

            std::unique_ptr<MYSQL, conn_deleter> conn_ptr(conn, conn_deleter());
            _connections.push(std::move(conn_ptr));
            successCount++;
        }
    } catch (std::exception& e) {
        while (!_connections.empty()) {
            auto conn = std::move(_connections.front());
            _connections.pop();
            if (conn) {
                mysql_close(conn.get());
            }
        }
        SPDLOG_ERROR("MysqlPool initialization failed: {}", e.what());
    }
    if (successCount < _poolSize) {
        SPDLOG_ERROR("Only {} Out Of {} Connections Were Established", successCount, _poolSize);
    }
}

MysqlPool::~MysqlPool()
{
    Close();
}

std::unique_ptr<MYSQL, MysqlPool::conn_deleter> MysqlPool::GetConnection() noexcept
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

void MysqlPool::ReturnConnection(std::unique_ptr<MYSQL, conn_deleter> conn) noexcept
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
    _pool = std::make_unique<MysqlPool>(host, user, password, schema);
}

MysqlDao::~MysqlDao()
{
    _pool->Close();
}

int MysqlDao::TestUidAndEmail(const std::string& uid, const std::string& email)
{
    auto conn = _pool->GetConnection();
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "select * from user where uid = ? and email = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            SPDLOG_WARN("mysql_stmt_prepare failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        MYSQL_BIND params[2];

        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char*)uid.c_str();
        params[0].buffer_length = uid.size();
        params[0].length = &params[0].buffer_length;

        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = (char*)email.c_str();
        params[1].buffer_length = email.size();
        params[1].length = &params[0].buffer_length;

        if (mysql_stmt_bind_param(stmt, params) != 0) {
            SPDLOG_WARN("mysql_stmt_bind_param failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            return -1;
        }

        // 关键：必须先存储结果集,之后才能查询行数
        if (mysql_stmt_store_result(stmt) != 0) {
            SPDLOG_WARN("mysql_stmt_store_result failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        my_ulonglong row_count = mysql_stmt_num_rows(stmt);
        mysql_stmt_close(stmt);

        // 如果存在记录，返回 -1 表示用户已存在
        if (row_count != 1) {
            return -1;
        }

        return 1;

    } catch (const std::exception& e) {
        SPDLOG_ERROR("MysqlDao::TestUidAndEmail failed: {}", e.what());
        return -1;
    }
}

int MysqlDao::RegisterUser(const std::string& name, const std::string& email, const std::string& password)
{
    auto conn = _pool->GetConnection();
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "select * from user where name = ? or email =?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            SPDLOG_WARN("mysql_stmt_prepare failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        MYSQL_BIND params[2];

        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char*)name.c_str();
        params[0].buffer_length = name.size();
        params[0].length = &params[0].buffer_length;

        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = (char*)email.c_str();
        params[1].buffer_length = email.size();
        params[1].length = &params[1].buffer_length;

        if (mysql_stmt_bind_param(stmt, params) != 0) {
            SPDLOG_WARN("mysql_stmt_bind_param failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            return -1;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            mysql_stmt_close(stmt);
            return -1;
        }

        my_ulonglong row_count = mysql_stmt_num_rows(stmt);
        mysql_stmt_close(stmt);

        // 如果存在记录，返回 -1 表示用户已存在
        if (row_count > 0) {
            return -1;
        }

        stmt = mysql_stmt_init(conn.get());
        query = "INSERT INTO user (name, email, password) VALUES (?, ?, ?)";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            SPDLOG_WARN("mysql_stmt_prepare failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        // 绑定插入参数
        MYSQL_BIND insert_params[3];
        memset(insert_params, 0, sizeof(insert_params));

        unsigned long name_len = name.size();
        insert_params[0].buffer_type = MYSQL_TYPE_STRING;
        insert_params[0].buffer = (char*)name.c_str();
        insert_params[0].buffer_length = name_len;
        insert_params[0].length = &name_len;

        unsigned long email_len = email.size();
        insert_params[1].buffer_type = MYSQL_TYPE_STRING;
        insert_params[1].buffer = (char*)email.c_str();
        insert_params[1].buffer_length = email_len;
        insert_params[1].length = &email_len;

        unsigned long password_len = password.size();
        insert_params[2].buffer_type = MYSQL_TYPE_STRING;
        insert_params[2].buffer = (char*)password.c_str();
        insert_params[2].buffer_length = password_len;
        insert_params[2].length = &password_len;

        if (mysql_stmt_bind_param(stmt, insert_params) != 0) {
            SPDLOG_WARN("mysql_stmt_bind_param failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            SPDLOG_WARN("mysql_stmt_execute failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        mysql_stmt_close(stmt);

        return 1; // 返回1表示注册成功

    } catch (const std::exception& e) {
        SPDLOG_ERROR("MysqlDao::RegisterUser failed: {}", e.what());
        return -1;
    }
}

int MysqlDao::ResetPassword(const std::string& email, const std::string& password)
{

    auto conn = _pool->GetConnection();
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "update user set password = ? where email = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            SPDLOG_WARN("mysql_stmt_prepare failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        MYSQL_BIND params[2];

        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char*)password.c_str();
        params[0].buffer_length = password.size();
        params[0].length = &params[0].buffer_length;

        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = (char*)email.c_str();
        params[1].buffer_length = email.size();
        params[1].length = &params[1].buffer_length;

        if (mysql_stmt_bind_param(stmt, params) != 0) {
            SPDLOG_WARN("mysql_stmt_bind_param failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            return -1;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            mysql_stmt_close(stmt);
            return -1;
        }

        my_ulonglong affected_rows = mysql_stmt_affected_rows(stmt);
        if (affected_rows == (my_ulonglong)-1) {
            SPDLOG_WARN("mysql_stmt_affected_rows failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return false;
        }

        mysql_stmt_close(stmt);
        return 1; // 返回1表示重置密码成功

    } catch (const std::exception& e) {
        SPDLOG_ERROR("MysqlDao::ResetPassword failed: {}", e.what());
        return -1;
    }
    return 0;
}

bool MysqlDao::CheckPwd(const std::string& user, const std::string& password, UserInfo& userInfo)
{
    auto conn = _pool->GetConnection();
    Defer defer([this, &conn]() {
        _pool->ReturnConnection(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "select uid,name,email from user where ( uid = ? OR email = ? ) AND password = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            SPDLOG_WARN("mysql_stmt_prepare failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return false;
        }

        MYSQL_BIND params[3];

        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char*)user.c_str();
        params[0].buffer_length = user.size();
        params[0].length = &params[0].buffer_length;

        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = (char*)user.c_str();
        params[1].buffer_length = user.size();
        params[1].length = &params[1].buffer_length;

        params[2].buffer_type = MYSQL_TYPE_STRING;
        params[2].buffer = (char*)password.c_str();
        params[2].buffer_length = password.size();
        params[2].length = &params[2].buffer_length;

        if (mysql_stmt_bind_param(stmt, params) != 0) {
            SPDLOG_WARN("mysql_stmt_bind_param failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return false;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            return false;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            mysql_stmt_close(stmt);
            return false;
        }

        int count = mysql_stmt_num_rows(stmt);
        if (count != 1) {
            mysql_stmt_close(stmt);
            return false;
        }

        // 绑定结果缓冲区
        MYSQL_BIND result_bind[3]; // 根据实际列数调整
        long uid;
        char name_buffer[70];
        char email_buffer[70];

        memset(result_bind, 0, sizeof(result_bind));

        // 绑定第一列（示例：email字段）
        result_bind[0].buffer_type = MYSQL_TYPE_LONG;
        result_bind[0].buffer = &uid;

        result_bind[1].buffer_type = MYSQL_TYPE_STRING;
        result_bind[1].buffer = name_buffer;
        result_bind[1].buffer_length = sizeof(name_buffer);
        result_bind[1].length = &result_bind[1].buffer_length;

        result_bind[2].buffer_type = MYSQL_TYPE_STRING;
        result_bind[2].buffer = email_buffer;
        result_bind[2].buffer_length = sizeof(email_buffer);
        result_bind[2].length = &result_bind[2].buffer_length;

        if (mysql_stmt_bind_result(stmt, result_bind) != 0) {
            SPDLOG_WARN("mysql_stmt_bind_result failed: {}", mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            return false;
        }

        if (mysql_stmt_fetch(stmt) != 0) {
            mysql_stmt_close(stmt);
            return false;
        }

        userInfo.email = email_buffer;
        userInfo.name = name_buffer;
        userInfo.uid = uid;

        mysql_stmt_close(stmt);
        return true; // 返回1表示重置密码成功

    } catch (const std::exception& e) {
        SPDLOG_ERROR("MysqlDao::CheckPwd failed: {}", e.what());
        return false;
    }
    return false;
}
