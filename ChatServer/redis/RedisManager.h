#ifndef REDISMANAGER_H
#define REDISMANAGER_H

#include "../global/Singleton.h"
#include <atomic>
#include <condition_variable>
#include <hiredis/hiredis.h>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class RedisPool {
public:
    RedisPool(std::size_t size = std::thread::hardware_concurrency(), const std::string& host = "127.0.0.1", int port = 6379, const std::string& password = "");
    ~RedisPool();
    redisContext* CreateConnection();
    redisContext* GetConnection();
    void ReturnConnection(redisContext* context);
    void Close();

private:
    std::string _host;
    std::size_t _port;
    std::size_t _size;
    std::string _password;
    std::atomic<bool> _stop;
    std::queue<redisContext*> _connections;
    std::mutex _mutex;
    std::condition_variable _cv;
};

class RedisManager : public Singleton<RedisManager> {
    friend class Singleton<RedisManager>;

public:
    ~RedisManager();
    bool Get(const std::string& key, std::string& value);
    bool Set(const std::string& key, const std::string& value);
    bool Auth(const std::string& password);
    bool LPush(const std::string& key, const std::string& value);
    bool LPop(const std::string& key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string& key, const std::string& hkey);
    bool Del(const std::string& key);
    bool ExistsKey(const std::string& key);
    void Close();
    inline bool isConnected() const { return _isConnected; }

private:
    RedisManager();
    template <typename... Args>
    redisReply* execute(const char* format, const Args&... args)
    {
        auto* context = _pool->GetConnection();
        redisReply* reply = (redisReply*)redisCommand(context, format, args...);
        _pool->ReturnConnection(context);
        return reply;
    }

private:
    std::string _host;
    std::size_t _port;
    bool _isConnected = false;
    std::unique_ptr<RedisPool> _pool;
};

#endif