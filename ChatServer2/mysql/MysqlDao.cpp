#include "MysqlDao.h"
#include "../data/UserInfo.h"
#include "../global/ConfigManager.h"
#include "../global/const.h"
#include <exception>
#include <iostream>
#include <mariadb_com.h>
#include <mysql.h>
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

int MysqlDao::TestUidAndEmail(const std::string& uid, const std::string& email)
{
    auto conn = _pool->GetConnection();
    try {
        if (conn == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "select * from user where uid = ? and email = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            //std::cout << mysql_error(conn.get()) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
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
            //std::cout << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        // 关键：必须先存储结果集,之后才能查询行数
        if (mysql_stmt_store_result(stmt) != 0) {
            //std::cout << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        my_ulonglong row_count = mysql_stmt_num_rows(stmt);
        mysql_stmt_close(stmt);

        // 如果存在记录，返回 -1 表示用户已存在
        if (row_count != 1) {
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        return 1;

    } catch (const std::exception& e) {
        if (conn != nullptr) {
            _pool->ReturnConnection(std::move(conn));
        }
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}

int MysqlDao::RegisterUser(const std::string& name, const std::string& email, const std::string& password)
{
    auto conn = _pool->GetConnection();
    try {
        if (conn == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "select * from user where name = ? or email =?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            //std::cout << mysql_error(conn.get()) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
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
            //std::cout << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        my_ulonglong row_count = mysql_stmt_num_rows(stmt);
        mysql_stmt_close(stmt);

        //std::cout << "count:" << row_count << std::endl;
        // 如果存在记录，返回 -1 表示用户已存在
        if (row_count > 0) {
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        stmt = mysql_stmt_init(conn.get());
        query = "INSERT INTO user (name, email, password) VALUES (?, ?, ?)";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            //std::cout << "Insert prepare failed: " << mysql_error(conn.get()) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

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
            //std::cout << "Insert bind failed: " << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            //std::cout << "Insert execute failed: " << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        mysql_stmt_close(stmt);
        _pool->ReturnConnection(std::move(conn));

        return 1; // 返回1表示注册成功

    } catch (const std::exception& e) {
        if (conn != nullptr) {
            _pool->ReturnConnection(std::move(conn));
        }
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}

int MysqlDao::ResetPassword(const std::string& email, const std::string& password)
{

    auto conn = _pool->GetConnection();
    try {
        if (conn == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "update user set password = ? where email = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            //std::cout << mysql_error(conn.get()) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
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
            //std::cout << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return -1;
        }

        my_ulonglong affected_rows = mysql_stmt_affected_rows(stmt);
        if (affected_rows == (my_ulonglong)-1) {
            std::cerr << "Error getting affected rows" << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        mysql_stmt_close(stmt);
        _pool->ReturnConnection(std::move(conn));
        return 1; // 返回1表示重置密码成功

    } catch (const std::exception& e) {
        if (conn != nullptr) {
            _pool->ReturnConnection(std::move(conn));
        }
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

bool MysqlDao::CheckPwd(const std::string& user, const std::string& password, UserInfo& userInfo)
{
    auto conn = _pool->GetConnection();
    try {
        if (conn == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
        std::string query = "select * from user where name = ? and password = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            //std::cout << mysql_error(conn.get()) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        MYSQL_BIND params[2];

        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char*)user.c_str();
        params[0].buffer_length = user.size();
        params[0].length = &params[0].buffer_length;

        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = (char*)password.c_str();
        params[1].buffer_length = password.size();
        params[1].length = &params[1].buffer_length;

        if (mysql_stmt_bind_param(stmt, params) != 0) {
            //std::cout << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        int count = mysql_stmt_num_rows(stmt);
        if (count != 1) {
            mysql_stmt_close(stmt);
            _pool->ReturnConnection(std::move(conn));
            return false;
        }

        mysql_stmt_close(stmt);
        _pool->ReturnConnection(std::move(conn));
        return true; // 返回1表示重置密码成功

    } catch (const std::exception& e) {
        if (conn != nullptr) {
            _pool->ReturnConnection(std::move(conn));
        }
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
    return false;
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
    auto conn = _pool->GetConnection();
    std::shared_ptr<UserInfo> user_info = nullptr;
    MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
    Defer defer([this, &conn, stmt]() {
        _pool->ReturnConnection(std::move(conn));
        mysql_stmt_close(stmt);
    });
    try {
        if (conn == nullptr || stmt == nullptr) {
            return user_info;
        }

        std::string query = "select name,email,password from user where uid = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            return user_info;
        }

        MYSQL_BIND params[1];
        std::string uid_str = std::to_string(uid);
        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char*)uid_str.c_str();
        params[0].buffer_length = uid_str.size();
        params[0].length = &params[0].buffer_length;

        if (mysql_stmt_bind_param(stmt, params) != 0) {
            return user_info;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            return user_info;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            return user_info;
        }

        int count = mysql_stmt_num_rows(stmt);
        if (count != 1) {
            return user_info;
        }

        const unsigned int field_count = mysql_stmt_field_count(stmt);
        if (field_count != 3) // pwd, name, email 暂时，之后添加
        {
            return user_info;
        }

        MYSQL_BIND result[3] = {};
        char res_name[70] = { 0 };
        char res_email[70] = { 0 };
        char res_password[70] = { 0 };
        result[0].buffer_type = MYSQL_TYPE_STRING;
        result[0].buffer = &res_name;
        result[0].buffer_length = sizeof(res_name);
        result[0].length = &result[0].buffer_length;
        // email
        result[1].buffer_type = MYSQL_TYPE_STRING;
        result[1].buffer = &res_email;
        result[1].buffer_length = sizeof(res_email);
        result[1].length = &result[1].buffer_length;
        // password
        result[2].buffer_type = MYSQL_TYPE_STRING;
        result[2].buffer = &res_password;
        result[2].buffer_length = sizeof(res_password);
        result[2].length = &result[2].buffer_length;

        if (mysql_stmt_bind_result(stmt, result) != 0) {
            return user_info;
        }

        if (mysql_stmt_fetch(stmt) == 0) {
            user_info = std::make_shared<UserInfo>();
            user_info->name = std::string(res_name);
            user_info->email = std::string(res_email);
            user_info->password = std::string(res_password);
        }

    } catch (const std::exception& e) {
        if (conn != nullptr) {
            _pool->ReturnConnection(std::move(conn));
        }
        std::cerr << "Exception: " << e.what() << std::endl;
        return nullptr;
    }
    return user_info;
}
std::shared_ptr<UserInfo> MysqlDao::GetUser(const std::string& name)
{
    auto conn = _pool->GetConnection();
    std::shared_ptr<UserInfo> user_info = nullptr;
    MYSQL_STMT* stmt = mysql_stmt_init(conn.get());
    Defer defer([this, &conn, stmt]() {
        _pool->ReturnConnection(std::move(conn));
        mysql_stmt_close(stmt);
    });
    try {
        if (conn == nullptr) {
            _pool->ReturnConnection(std::move(conn));
            return user_info;
        }

        std::string query = "select name,email,password from user where uid = ?";

        if (mysql_stmt_prepare(stmt, query.c_str(), query.size()) != 0) {
            return user_info;
        }

        MYSQL_BIND params[1];
        memset(params, 0, sizeof(params));
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char*)name.c_str();
        params[0].buffer_length = name.size();
        params[0].length = &params[0].buffer_length;

        if (mysql_stmt_bind_param(stmt, params) != 0) {
            return user_info;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            return user_info;
        }

        if (mysql_stmt_store_result(stmt) != 0) {
            return user_info;
        }

        int count = mysql_stmt_num_rows(stmt);
        if (count != 1) {
            return user_info;
        }

        const unsigned int field_count = mysql_stmt_field_count(stmt);
        if (field_count != 3) // pwd, name, email 暂时，之后添加
        {
            return user_info;
        }

        MYSQL_BIND result[3] = {};
        char res_name[70] = { 0 };
        char res_email[70] = { 0 };
        char res_password[70] = { 0 };
        result[0].buffer_type = MYSQL_TYPE_STRING;
        result[0].buffer = &res_name;
        result[0].buffer_length = sizeof(res_name);
        result[0].length = &result[0].buffer_length;
        // email
        result[1].buffer_type = MYSQL_TYPE_STRING;
        result[1].buffer = &res_email;
        result[1].buffer_length = sizeof(res_email);
        result[1].length = &result[1].buffer_length;
        // password
        result[2].buffer_type = MYSQL_TYPE_STRING;
        result[2].buffer = &res_password;
        result[2].buffer_length = sizeof(res_password);
        result[2].length = &result[2].buffer_length;

        if (mysql_stmt_bind_result(stmt, result) != 0) {
            return user_info;
        }

        if (mysql_stmt_fetch(stmt) == 0) {
            user_info->name = res_name;
            user_info->email = res_email;
            user_info->password = res_password;
        }

    } catch (const std::exception& e) {
        if (conn != nullptr) {
            _pool->ReturnConnection(std::move(conn));
        }
        std::cerr << "Exception: " << e.what() << std::endl;
        return nullptr;
    }
    return user_info;
}
