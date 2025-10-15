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

enum class ErrorCodes{
    SUCCESS = 0,
    ERROR_JSON = 1,
    ERROR_NETWORK = 2
};

extern QString gate_url_prefix;

#endif // GLOBAL_H
