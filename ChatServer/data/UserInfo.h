#pragma once
#include <string>

struct UserInfo{
UserInfo()
    : name("")
    , password("")
    , email("")
    , nick("")
    , desc("")
    , icon("")
    , back("")
    , sex(0)
    , uid(0){}

std::string name;
std::string password;
std::string email;
std::string nick;
std::string desc;
std::string icon;
std::string back;
int uid;
int sex;
};
