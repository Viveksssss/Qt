#include "MysqlDao.h"
#include "../global/ConfigManager.h"
#include <iostream>
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
        std::cerr << e.what() << std::endl;
    }
    if (successCount < _poolSize) {
        std::cerr << "Warning Only" << successCount << "Out Of" << _poolSize << " Connections Were Established" << std::endl;
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

int MysqlDao::RegisterUser(const std::string& name, const std::string& email, const std::string& password)
{
    auto conn = _pool->GetConnection();
    try {
        if (conn == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        std::string callSql = "CALL register_user(?, ?, ?), @result";
        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        if (stmt == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }
        if (mysql_stmt_prepare(stmt, callSql.c_str(), callSql.size()) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }
        MYSQL_BIND bind[3];
        std::string name_str = name;
        std::string email_str = email;
        std::string password_str = password;

        // 绑定 name 参数
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (char*)name_str.c_str();
        bind[0].buffer_length = name_str.length();
        bind[0].length = &bind[0].buffer_length;

        // 绑定 email 参数
        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (char*)email_str.c_str();
        bind[1].buffer_length = email_str.length();
        bind[1].length = &bind[1].buffer_length;

        // 绑定 password 参数
        bind[2].buffer_type = MYSQL_TYPE_STRING;
        bind[2].buffer = (char*)password_str.c_str();
        bind[2].buffer_length = password_str.length();
        bind[2].length = &bind[2].buffer_length;

        if (mysql_stmt_bind_param(stmt, bind) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        // 执行存储过程
        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        // 清理语句
        mysql_stmt_close(stmt);

        // 获取输出参数 @result
        if (mysql_query(conn.get(), "SELECT @result AS result") != 0) {
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        MYSQL_RES* result = mysql_store_result(conn.get());
        if (result == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        if (row == nullptr) {
            mysql_free_result(result);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        int reg_result = std::stoi(row[0]);

        mysql_free_result(result);
        _pool->ReturnConnection(std::move(conn));

        return reg_result;
    } catch (const std::exception& e) {
        if (conn != nullptr) {
            _pool->ReturnConnection(std::move(conn));
        }
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}
