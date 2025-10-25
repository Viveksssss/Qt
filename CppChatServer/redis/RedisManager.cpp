#include "RedisManager.h"
#include "../global/ConfigManager.h"
#include <spdlog/spdlog.h>
#include <string>

RedisManager::~RedisManager()
{
    if (_isConnected)
        Close();
}

RedisManager::RedisManager()
{
    _host = ConfigManager::GetInstance()["Redis"]["host"];
    _port = std::stoi(ConfigManager::GetInstance()["Redis"]["port"]);
    std::string password = ConfigManager::GetInstance()["Redis"]["password"];
    _pool = std::make_unique<RedisPool>(std::thread::hardware_concurrency(), _host, _port, password);
    _isConnected = true;
}

bool RedisManager::Get(const std::string& key, std::string& value)
{
    auto* reply = execute("GET %s", key.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR || reply->type == REDIS_REPLY_NIL) {
        SPDLOG_WARN("GET failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    value = std::string(reply->str, reply->len);
    freeReplyObject(reply);
    return true;
}

bool RedisManager::Set(const std::string& key, const std::string& value)
{
    auto* reply = execute("SET %s %s", key.c_str(), value.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("SET failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("SET {} = {}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisManager::Auth(const std::string& password)
{
    auto* reply = execute("AUTH %s", password.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("AUTH failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("AUTH OK");
    freeReplyObject(reply);
    return true;
}

bool RedisManager::LPush(const std::string& key, const std::string& value)
{
    auto* reply = execute("LPUSH %s %s", key.c_str(), value.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("LPUSH failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("LPUSH {} = {}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisManager::LPop(const std::string& key, std::string& value)
{
    auto* reply = execute("LPOP %s", key.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("LPOP failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    value = std::string(reply->str, reply->len);
    freeReplyObject(reply);
    SPDLOG_INFO("LPOP {} = {}", key, value);
    return true;
}

bool RedisManager::RPush(const std::string& key, const std::string& value)
{
    auto* reply = execute("RPUSH %s %s", key.c_str(), value.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("RPUSH failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("RPUSH {} = {}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisManager::RPop(const std::string& key, std::string& value)
{
    auto* reply = execute("RPOP %s", key.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("RPOP failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    value = std::string(reply->str, reply->len);
    freeReplyObject(reply);
    SPDLOG_INFO("RPOP {} = {}", key, value);
    return true;
}

bool RedisManager::HSet(const std::string& key, const std::string& hkey, const std::string& value)
{
    auto* reply = execute("HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("HSET failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("HSET {} {} = {}", key, hkey, value);
    freeReplyObject(reply);
    return true;
}

bool RedisManager::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
    auto* reply = execute("HSET %s %s %b", key, hkey, hvalue, hvaluelen);
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("HSET failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("HSET {} {} = {}", key, hkey, hvalue);
    freeReplyObject(reply);
    return true;
}

std::string RedisManager::HGet(const std::string& key, const std::string& hkey)
{
    auto* reply = execute("HGET %s %s", key.c_str(), hkey.c_str());
    if (reply == NULL) {
        return "";
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("HGET failed: {}", reply->str);
        freeReplyObject(reply);
        return "";
    }
    std::string value = std::string(reply->str, reply->len);
    freeReplyObject(reply);
    SPDLOG_INFO("HGET {} {} = {}", key, hkey, value);
    return value;
}

bool RedisManager::Del(const std::string& key)
{
    auto* reply = execute("DEL %s", key.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("DEL failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("DEL {}", key);
    freeReplyObject(reply);
    return true;
}

bool RedisManager::ExistsKey(const std::string& key)
{
    auto* reply = execute("EXISTS %s", key.c_str());
    if (reply == NULL) {
        return false;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        SPDLOG_WARN("EXISTS failed: {}", reply->str);
        freeReplyObject(reply);
        return false;
    }
    bool exists = (reply->integer == 1);
    freeReplyObject(reply);
    SPDLOG_INFO("EXISTS {} = {}", key, exists ? "true" : "false");
    return exists;
}

void RedisManager::Close()
{
    if (_pool) {
        _pool->Close();
        _isConnected = false;
        _pool.reset();
    }
    SPDLOG_INFO("Redis connection closed");
}

RedisPool::RedisPool(std::size_t size, const std::string& host, int port, const std::string& password)
    : _size(size)
    , _host(host)
    , _port(port)
    , _password(password)
    , _stop(false)
{
    bool success = true;
    for (std::size_t i = 0; i < _size; ++i) {
        auto* context = CreateConnection();
        if (context == nullptr) {
            success = false;
            break;
        }
        if (context) {
            _connections.push(context);
        }
    }
    if (!success) {
        SPDLOG_ERROR("Failed to connect to Redis server");
        exit(1);
    }
}

RedisPool::~RedisPool()
{
    Close();
}

redisContext* RedisPool::CreateConnection()
{
    redisContext* context = redisConnect(_host.c_str(), _port);
    if (context == NULL || context->err) {
        if (context)
            redisFree(context);
        SPDLOG_ERROR("Config error or something else went wrong");
        return nullptr;
    }
    if (!_password.empty()) {
        auto reply = (redisReply*)redisCommand(context, "AUTH %s", _password.c_str());
        if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
            SPDLOG_WARN("AUTH failed: {}", reply->str);
            redisFree(context);
            freeReplyObject(reply);
            return nullptr;
        }
    }
    return context;
}

redisContext* RedisPool::GetConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this] { return !_connections.empty() || _stop; });
    if (_stop) {
        return nullptr;
    }
    auto* context = _connections.front();
    _connections.pop();
    lock.unlock();
    if (context->err) {
        redisFree(context);
        context = CreateConnection();
    }
    return context;
}

void RedisPool::ReturnConnection(redisContext* context)
{
    if (context == nullptr)
        return;
    if (context->err) {
        redisFree(context);
        context = CreateConnection();
        if (!context) {
            SPDLOG_WARN("Failed to recreate broken connection");
            return; // 连接池大小暂时减少
        }
    }
    if (context) {
        std::unique_lock<std::mutex> lock(_mutex);
        _connections.push(context);
        _cv.notify_one();
        /*
        在生产者-消费者模式中，先通知后解锁是为了避免丢失唤醒和竞态条件。

        如果先解锁后通知：

        解锁后，其他线程可能立即获取锁并消费资源

        然后才执行通知，这时等待的线程被唤醒但资源已被抢走

        导致虚假唤醒或永久等待

        在锁内通知能保证：线程被唤醒时，资源肯定还在队列中，状态一致性得到保障。这是条件变量的标准用法。
        */
    }
}

void RedisPool::Close()
{
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_stop) {
            return;
        }
        _stop = true;
        while (!_connections.empty()) {
            auto* context = _connections.front();
            _connections.pop();
            if (context) {
                redisFree(context);
            }
        }
    }
    _cv.notify_all();
    SPDLOG_INFO("Redis pool closed");
}