#include "../global/Singleton.h"
#include "../global/const.h"
#include "../session/Session.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

typedef std::function<void(std::shared_ptr<Session>, uint16_t msg_id, const std::string& msg)> FuncBack;

class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;

public:
    void PostMsgToQueue(std::shared_ptr<LogicNode> msg);
    void RegisterCallBacks();
    void DealMsg();

public:
    LogicSystem(std::size_t size = std::thread::hardware_concurrency());
    ~LogicSystem();

private:
    std::queue<std::shared_ptr<LogicNode>> _queue;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::vector<std::thread> _work_threads;
    std::size_t _size;
    bool _stop;
    std::unordered_map<MsgId, FuncBack> _function_callbacks;
};