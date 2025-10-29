#ifndef CONST_H
#define CONST_H

#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define MAX_LENGTH 65535
#define MAX_SEND_SIZE 5000
#define MAX_RECV_SIZE 10000

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using json = nlohmann::json;

enum class RequestType {
    GET,
    POST
};

enum class MsgId {
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002, // 注册用户
    ID_RESET_PWD = 1003, // 重置密码
    ID_LOGIN_USER = 1004, // 用户登录
    ID_CHAT_LOGIN = 1005, // 登陆聊天服务器
    ID_CHAT_LOGIN_RSP = 1006, // 登陆聊天服务器回包
    ID_SEARCH_USER_REQ = 1007, // 用户搜索请求
    ID_SEARCH_USER_RSP = 1008, // 搜索用户回包
    ID_ADD_FRIEND_REQ = 1009, // 添加好友申请
    ID_ADD_FRIEND_RSP = 1010, // 申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011, // 通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013, // 认证好友请求
    ID_AUTH_FRIEND_RSP = 1014, // 认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, // 通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ = 1017, // 文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1018, // 文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 通知用户文本聊天信息
    ID_NOTIFY_OFF_LINE_REQ = 1021, // 通知用户下线
    ID_HEART_BEAT_REQ = 1023, // 心跳请求
    ID_HEARTBEAT_RSP = 1024, // 心跳回复
};

enum class ErrorCodes {
    SUCCESS = 0,
    ERROR_NETWORK = 1001,
    ERROR_JSON = 1002,
    RPCFAILED = 1003,
    ERROR_SECURITYCODE_EXPIRED = 1004,
    ERROR_SECURITYCODE_NOTFOUND = 1005,
    ERROR_EMAIL_NOTFOUND = 1006,
    ERROR_USER_OR_PASSWORD_INCORRECT = 1007,
};

class ConfigManager;

class Defer {
public:
    Defer(std::function<void()> func);
    ~Defer();

private:
    std::function<void()> m_func;
};

extern ConfigManager cfgMgr;

#define EMAIL_PREFIX "email_"
#define USER_PREFIX "user_"
#endif // CONST_H