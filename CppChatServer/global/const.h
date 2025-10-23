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
};

class ConfigManager;
extern ConfigManager cfgMgr;

#define EMAIL_PREFIX "email_"
#define USER_PREFIX "user_"
#endif // CONST_H