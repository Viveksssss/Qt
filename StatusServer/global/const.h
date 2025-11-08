#ifndef CONST_H
#define CONST_H

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

enum class ErrorCodes {
    SUCCESS = 0,
    ERROR_NETWORK = 1001,
    ERROR_JSON = 1002,
    RPCFAILED = 1003,
    ERROR_SECURITYCODE_EXPIRED = 1004,
    ERROR_SECURITYCODE_NOTFOUND = 1005,
    ERROR_EMAIL_NOTFOUND = 1006,
    ERROR_USER_OR_PASSWORD_INCORRECT = 1007,
    ERROR_UID_INVALID = 1008,
    ERROR_TOKEN_INVALID = 1009,
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
#define USERIP_PREFIX "uip_"
#define USER_TOKEN_PREFIX "user_token_"
#define LOGIN_COUNT_PREFIX "login_count_"
#define USER_BASE_INFO_PREFIX "user_base_info_"
#define IP_COUNT_PREFIX "ip_count_"
#endif // CONST_H
