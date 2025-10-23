#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <mutex>
#include <memory>
#include <QString>
#include <functional>

enum class RequestType{
    GET_SECURITY_CODE = 1001,
    REG_USER = 1002,
    FORGOT_PWD = 1003,
};

enum class Modules{
    REGISTERMOD = 0,
    FORGOTMOD = 1,
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

enum class RegisterVarify{
    ERROR_CONTENT_INCOMPLETE,
    ERROR_EMAIL_INCORRECTFORMAT,
    ERROR_PASSWORD_NOTSURE,
    ERROR_PASSWORD_LEN,
    ERROR_SECURITY_EMPTY,
    SUCCESS
};

extern QString gate_url_prefix;

extern std::function<QString(QString)>cryptoString;

#endif // GLOBAL_H
