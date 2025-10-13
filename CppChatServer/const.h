#ifndef CONST_H
#define CONST_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>

#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#elif __has_include(<nlohmann_json/json.hpp>)
#include <nlohmann_json/json.hpp>
#else
#error "nlohmann/json library not found!"
#endif

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
    ERROR_JSON = 1001,
    RPCFAILED = 1002
};

#endif // CONST_H