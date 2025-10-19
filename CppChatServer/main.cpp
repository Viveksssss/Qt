#include "ConfigManager.h"
#include "GateWayServer.h"
#include "RedisManager.h"
#include "VerifyClient.h"
#include <boost/asio.hpp>
#include <hiredis/hiredis.h>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void TestRedis()
{
    assert(RedisManager::GetInstance()->Auth("38324"));
    assert(RedisManager::GetInstance()->Set("blogwebsite", "llfc.club"));
    std::string value = "";
    assert(RedisManager::GetInstance()->Get("blogwebsite", value));
    // assert(RedisManager::GetInstance()->Get("nonekey", value) == false);
    assert(RedisManager::GetInstance()->HSet("bloginfo", "blogwebsite", "llfc.club"));
    assert(RedisManager::GetInstance()->HGet("bloginfo", "blogwebsite") != "");
    assert(RedisManager::GetInstance()->ExistsKey("bloginfo"));
    assert(RedisManager::GetInstance()->Del("bloginfo"));
    assert(RedisManager::GetInstance()->Del("bloginfo"));
    assert(RedisManager::GetInstance()->ExistsKey("bloginfo") == false);
    assert(RedisManager::GetInstance()->LPush("lpushkey1", "lpushvalue1"));
    assert(RedisManager::GetInstance()->LPush("lpushkey1", "lpushvalue2"));
    assert(RedisManager::GetInstance()->LPush("lpushkey1", "lpushvalue3"));
    assert(RedisManager::GetInstance()->RPop("lpushkey1", value));
    assert(RedisManager::GetInstance()->RPop("lpushkey1", value));
    assert(RedisManager::GetInstance()->LPop("lpushkey1", value));
    // assert(RedisManager::GetInstance()->LPop("lpushkey2", value) == false);
    RedisManager::GetInstance()->Close();
}

int main(int, char**)
{

    TestRedis();

    // auto& cfgMgr = ConfigManager::GetInstance();
    // std::cout << cfgMgr["GateWayServer"]["port"] << std::endl; // prints 9999

    // try {
    //     unsigned short port = 9999;
    //     net::io_context ioc;
    //     net::signal_set signals(ioc, SIGINT, SIGTERM);
    //     signals.async_wait([&ioc](const boost::system::error_code&, int) { ioc.stop(); });
    //     std::make_shared<GateWayServer>(ioc, port)->Start();
    //     ioc.run();
    // } catch (std::exception& e) {
    //     std::cerr << "Exception: " << e.what() << "\n";
    //     return 1;
    // }
}
