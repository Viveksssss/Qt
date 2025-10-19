#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <mutex>
#include <memory>
#include <QString>

enum class RequestType{
    GET_SECURITY_CODE = 1001,
    REG_USER = 1002,
};

enum class Modules{
    REGISTERMOD = 0,
};

enum class ErrorCodes {
    SUCCESS = 0,
    ERROR_NETWORK = 1001,
    ERROR_JSON = 1002,
    RPCFAILED = 1003,
    ERROR_SECURITYCODE_EXPIRED = 1004,
    ERROR_SECURITYCODE_NOTFOUND = 1005
};

enum class RegisterVarify{
    CONTENT_INCOMPLETE,
    EMAIL_INCORRECTFORMAT,
    PASSWORD_NOTSURE,
    SUCCESS
};

extern QString gate_url_prefix;

#endif // GLOBAL_H
