#include "LogicSystem.h"
#include "Session.h"
#include "VerifyClient.h"
#include <iostream>
#include <regex>

LogicSystem::LogicSystem()
{
    RegistHandlers("/get_test", RequestType::GET, [this](std::shared_ptr<Session> session) { 
        beast::ostream(session->_response.body()) << "receive login request\n"; 
        int i = 0;
        for(const auto&ele:session->_get_params ){
            i++;
            beast::ostream(session->_response.body()) << i <<"\t" << ele.first << ":" << ele.second << "\n";
        } });

    RegistHandlers("/getSecurityCode", RequestType::POST, [this](std::shared_ptr<Session> session) {
        auto body_str = beast::buffers_to_string(session->_request.body().data());
        std::cout << "receive getSecurityCode request, body: " << body_str << std::endl;
        session->_response.set(http::field::content_type, "text/json");

        // json解析
        json j = json::parse(body_str);
        if (j.is_discarded()) {
            std::cerr << "无效json" << std::endl;
            j["error"] = ErrorCodes::ERROR_JSON;
            std::string returnJson = j.dump(4);
            beast::ostream(session->_response.body()) << returnJson;
            return true;
        }

        auto sendError = [&](ErrorCodes error_code, const std::string& message) {
            json error_response = {
                { "success", false },
                { "error", error_code },
                { "message", message }
            };
            beast::ostream(session->_response.body()) << error_response.dump(4);
            return true; // 返回true表示请求处理完成
        };

        if (!j.contains("email")) {
            return sendError(ErrorCodes::RPCFAILED, "email is required");
        }

        auto email = j["email"].get<std::string>();

        // 验证格式
        std::regex email_regex(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b)");
        if (!std::regex_match(email, email_regex)) {
            return sendError(ErrorCodes::RPCFAILED, "email format error");
        }

        GetSecurityCodeResponse response = VerifyClient::GetInstance()->GetSecurityCode(email);

        // 发送验证码
        json returnJson = {
            { "success", true },
            { "error", ErrorCodes::SUCCESS },
            { "message", "okle!" }
        };
        beast::ostream(session->_response.body()) << returnJson.dump(4);
        return true;
    });
}

bool LogicSystem::HandleGet(const std::string& route, std::shared_ptr<Session> handler_ptr)
{
    if (_get_handlers.find(route) == _get_handlers.end()) {
        return false;
    }
    _get_handlers[route](handler_ptr);
    return true;
}

bool LogicSystem::HandlePost(const std::string& route, std::shared_ptr<Session> handler_ptr)
{
    if (_post_handlers.find(route) == _post_handlers.end()) {
        return false;
    }
    _post_handlers[route](handler_ptr);
    return true;
}

void LogicSystem::RegistHandlers(const std::string& route, RequestType type, SessionHandler handler)
{
    type == RequestType::GET ? _get_handlers[route] = handler : _post_handlers[route] = handler;
}
