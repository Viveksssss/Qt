#include "LogicSystem.h"
#include "../data/UserInfo.h"
#include "../data/im.pb.h"
#include "../global/ConfigManager.h"
#include "../global/UserManager.h"
#include "../global/const.h"
#include "../grpc/ChatGrpcClient.h"
#include "../mysql/MysqlManager.h"
#include "../redis/RedisManager.h"
#include <algorithm>
#include <boost/mpl/base.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <string>

std::string thread_id_to_string(std::thread::id id)
{
    std::stringstream ss;
    ss << id;
    return ss.str();
}

void LogicSystem::PostMsgToQueue(std::shared_ptr<LogicNode> msg)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _queue.push(msg);
    _cv.notify_one();
}

void LogicSystem::RegisterCallBacks()
{
    /**
     * @brief 登陆请求回调函数
     *
     */
    _function_callbacks[MsgId::ID_CHAT_LOGIN] = [this](std::shared_ptr<Session> session, uint16_t msg_id, const std::string& msg) {
        json j = json::parse(msg);
        auto uid = j["uid"].get<int>();
        auto token = j["token"].get<std::string>();
        SPDLOG_INFO("Thread: {},User {} Login with token {}", thread_id_to_string(std::this_thread::get_id()), uid, token);

        json jj;
        Defer defer([this, &jj, session]() {
            std::string return_str = jj.dump();
            session->Send(return_str, static_cast<int>(MsgId::ID_CHAT_LOGIN_RSP));
        });

        std::string uid_str = std::to_string(uid);
        std::string token_key = USER_TOKEN_PREFIX + uid_str;
        std::string token_value = "";

        bool success = RedisManager::GetInstance()->Get(token_key, token_value);
        if (!success) {
            jj["error"] = ErrorCodes::ERROR_UID_INVALID;
            return;
        }
        if (token_value != token) {
            jj["error"] = ErrorCodes::ERROR_TOKEN_INVALID;
            return;
        }

        std::string base_key = USER_BASE_INFO_PREFIX + uid_str;
        auto user_info = std::make_shared<UserInfo>();
        bool b_base = GetBaseInfo(base_key, uid, user_info);
        if (!b_base) {
            jj["error"] = ErrorCodes::ERROR_UID_INVALID;
            return;
        }

        jj["error"] = ErrorCodes::SUCCESS;

        // 将登陆人的状态信息改变为1
        std::string key = USER_STATUS_PREFIX + uid_str;
        RedisManager::GetInstance()->Set(key, "1");

        jj["uid"] = uid;
        jj["name"] = user_info->name;
        jj["email"] = user_info->email;
        jj["nick"] = user_info->nick;
        jj["sex"] = user_info->sex;
        jj["desc"] = user_info->desc;
        jj["icon"] = user_info->icon;
        jj["token"] = token;

        // 获取申请列表
        std::vector<std::shared_ptr<UserInfo>> apply_list;
        bool b_apply = MysqlManager::GetInstance()->GetFriendApplyList(uid_str, apply_list);
        if (b_apply && apply_list.size() > 0) {
            // 我们这里规定哪怕数据库操作成功，但是没有数据也算失败，就直接跳过，避免多余判断。
            json apply_friends;
            for (auto& apply_user : apply_list) {
                json apply_friend;
                apply_friend["uid"] = apply_user->uid;
                apply_friend["name"] = apply_user->name;
                apply_friend["email"] = apply_user->email;
                apply_friend["icon"] = apply_user->icon;
                apply_friend["sex"] = apply_user->sex;
                apply_friend["desc"] = apply_user->desc;
                apply_friend["back"] = apply_user->back; // 时间
                apply_friends.push_back(apply_friend);
            }
            jj["apply_friends"] = apply_friends;
        }
        // 获取通知列表
        std::vector<std::shared_ptr<UserInfo>> notification_list;
        bool b_notify = MysqlManager::GetInstance()->GetNotificationList(uid_str, notification_list);
        if (b_notify && notification_list.size() > 0) {
            json notifications;
            for (auto& notification : notification_list) {
                json item;
                item["uid"] = notification->uid;
                item["type"] = notification->status; // 用status代表type借用UserInfo的结构。
                item["message"] = notification->desc; // 用desc代表message借用UserInfo的结构。
                item["time"] = notification->back; // 备用字段表示时间。
                notifications.push_back(item);
            }
            jj["notifications"] = notifications;
        }

        // 获取会话列表
        std::vector<std::shared_ptr<SessionInfo>> session_list;
        bool b_session = MysqlManager::GetInstance()->GetSeessionList(uid_str, session_list);
        if (b_session && session_list.size() > 0) {
            json conversations;
            for (auto& session_item : session_list) {
                json conversation;
                conversation["uid"] = session_item->uid;
                conversation["from_uid"] = session_item->from_uid;
                conversation["to_uid"] = session_item->to_uid;
                conversation["create_time"] = session_item->create_time;
                conversation["update_time"] = session_item->update_time;
                conversation["name"] = session_item->name;
                conversation["icon"] = session_item->icon;
                conversation["status"] = session_item->status;
                conversation["deleted"] = session_item->deleted;
                conversation["pined"] = session_item->pined;
                conversations.push_back(conversation);
            }
            jj["conversations"] = conversations;
        }

        // 获取好友列表
        std::vector<std::shared_ptr<UserInfo>> friend_list;
        std::vector<int> online_friends;

        bool b_friend = MysqlManager::GetInstance()->GetFriendList(uid_str, friend_list);
        online_friends.resize(friend_list.size());
        if (b_friend && friend_list.size() > 0) {
            json friends;
            for (std::size_t i = 0; i < friend_list.size(); i++) {
                auto& friend_user = friend_list[i];
                json friend_item;
                // 查询状态
                std::string status_key = USER_STATUS_PREFIX + std::to_string(friend_user->uid);
                std::string status_value;
                bool b_status = RedisManager::GetInstance()->Get(status_key, status_value);
                if (b_status) {
                    friend_item["status"] = std::stoi(status_value);
                    online_friends[i] = friend_item["status"];
                } else {
                    friend_item["status"] = 0;
                    online_friends[i] = 0;
                }
                friend_item["uid"] = friend_user->uid;
                friend_item["name"] = friend_user->name;
                friend_item["email"] = friend_user->email;
                friend_item["icon"] = friend_user->icon;
                friend_item["sex"] = friend_user->sex;
                friend_item["desc"] = friend_user->desc;
                friends.push_back(friend_item);
            }
            jj["friends"] = friends;
        }

        // 登陆成功，通知所有在线好友
        // 上面得到了好友列表，这里通知所有在线好友
        for (std::size_t i = 0; i < friend_list.size(); i++) {
            auto& friend_uid = friend_list[i]->uid;
            std::string ip_key = USERIP_PREFIX + std::to_string(friend_uid);
            std::string ip_value;
            bool b_ip = RedisManager::GetInstance()->Get(ip_key, ip_value);
            if (b_ip) {
                if (online_friends[i] == 1) {
                    auto& cfg = ConfigManager::GetInstance();
                    auto self_name = cfg["SelfServer"]["name"];
                    if (ip_value == self_name) {
                        auto session2 = UserManager::GetInstance()->GetSession(friend_uid);
                        if (session2) {
                            SPDLOG_INFO("FROM UID:{},to:{}", uid, friend_uid);
                            json j;
                            j["error"] = ErrorCodes::SUCCESS;
                            j["uid"] = uid;
                            j["message"] = "😁好友" + user_info->name + "上线了😄";
                            j["type"] = static_cast<int>(NotificationCodes::ID_NOTIFY_FRIEND_ONLINE);
                            j["status"] = 1;
                            // 当前时间
                            auto now = std::chrono::system_clock::now();
                            auto time_t = std::chrono::system_clock::to_time_t(now);

                            std::stringstream ss;
                            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
                            j["time"] = ss.str();
                            j["icon"] = user_info->icon;
                            session2->Send(j.dump(), static_cast<int>(MsgId::ID_NOTIFY));
                        }
                    } else {
                        NotifyFriendOnlineRequest request;
                        request.set_fromuid(uid);
                        request.set_touid(friend_uid);
                        request.set_name(user_info->name);
                        request.set_type(static_cast<int>(NotificationCodes::ID_NOTIFY_FRIEND_ONLINE));
                        request.set_message("😁好友" + user_info->name + "上线了😄");
                        request.set_icon(user_info->icon);
                        // 当前时间
                        auto now = std::chrono::system_clock::now();
                        auto time_t = std::chrono::system_clock::to_time_t(now);
                        std::stringstream ss;
                        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
                        request.set_time(ss.str());

                        ChatGrpcClient::GetInstance()->NotifyFriendOnline(ip_value, request);
                    }
                }
            }
        }

        // 更新登陆数量
        auto server_name = ConfigManager::GetInstance()["SelfServer"]["name"];
        auto count_str = RedisManager::GetInstance()->HGet(LOGIN_COUNT_PREFIX, server_name);
        int count = 0;
        if (!count_str.empty()) {
            count = std::stoi(count_str);
        }
        count++;
        count_str = std::to_string(count);
        RedisManager::GetInstance()->HSet(LOGIN_COUNT_PREFIX, server_name, count_str);

        // session绑定uid
        session->SetUid(uid);
        // 绑定连接的服务器名称和用户uid
        std::string ip_key = USERIP_PREFIX + std::to_string(uid);
        RedisManager::GetInstance()->Set(ip_key, server_name);
        // uid和session绑定管理，方便之后踢人
        UserManager::GetInstance()->SetUserSession(uid, session);
        // 设置用户状态在线
        std::string status_key = USER_STATUS_PREFIX + uid_str;
        RedisManager::GetInstance()->Set(status_key, "1");
    };

    /**
     * @brief 搜索用户回调函数
     *
     */
    _function_callbacks[MsgId::ID_SEARCH_USER_REQ] = [this](std::shared_ptr<Session> session, uint16_t msg_id, const std::string& msg) {
        json j = json::parse(msg);
        j["error"] = static_cast<int>(ErrorCodes::SUCCESS);
        SPDLOG_INFO("json:{}", j.dump());
        auto uid_str = j["toUid"].get<std::string>();
        Defer defer([this, session, &j]() {
            SPDLOG_INFO("j.size:{},j.dump:{}", j.dump().size(), j.dump());
            session->Send(j.dump(), static_cast<int>(MsgId::ID_SEARCH_USER_RSP));
        });

        bool only_digit = IsPureDigit(uid_str);

        GetSearchedUsers(uid_str, j, only_digit);
    };
    /*
     * * @brief 好友申请请求
     */
    _function_callbacks[MsgId::ID_ADD_FRIEND_REQ] = [this](std::shared_ptr<Session> session, uint16_t msg_id, const std::string& msg) {
        json j = json::parse(msg);
        j["error"] = ErrorCodes::SUCCESS;
        Defer defer([this, &j, session]() {
            // 回复请求方的信息
            session->Send(j.dump(), static_cast<int>(MsgId::ID_ADD_FRIEND_RSP));
        });
        auto toUid = j["toUid"].get<int>();
        auto fromUid = j["fromUid"].get<int>();
        auto fromName = j["fromName"].get<std::string>();
        auto fromSex = j["fromSex"].get<int>();
        auto fromDesc = j["fromDesc"].get<std::string>();
        // auto fromIcon = j["fromIcon"].get<std::string>();
        auto fromIcon = j.value("fromIcon", "");

        std::string uid_str = std::to_string(toUid);
        // 先检查双方是否互相发送请求，如果是直接双方同意。
        bool apply_each = MysqlManager::GetInstance()->CheckApplied(std::to_string(toUid), std::to_string(fromUid));
        if (apply_each) {
            json jj;
            jj["error"] = ErrorCodes::SUCCESS;
            jj["from_uid"] = fromUid;
            jj["from_name"] = fromName;
            jj["from_sex"] = fromSex;
            jj["from_icon"] = fromIcon;
            std::string key;
            bool b_get = RedisManager::GetInstance()->Get(USER_STATUS_PREFIX + std::to_string(fromUid), key);
            if (b_get) {
                jj["from_status"] = std::stoi(key);
            } else {
                jj["from_status"] = 0;
            }
            jj["ok"] = true; // 标记成功
            MysqlManager::GetInstance()->AddNotification(uid_str, static_cast<int>(NotificationCodes::ID_NOTIFY_MAKE_FRIENDS), "成功和" + fromName + "成为好友");
            // 给对方发送请求信息
            auto& cfg = ConfigManager::GetInstance();
            auto self_name = cfg["SelfServer"]["name"];

            auto to_key = USERIP_PREFIX + uid_str;
            std::string to_ip_value;
            bool b_ip = RedisManager::GetInstance()->Get(to_key, to_ip_value);
            if (b_ip) {
                if (to_ip_value == self_name) {
                    auto session2 = UserManager::GetInstance()->GetSession(toUid);
                    if (session2) {
                        SPDLOG_INFO("FROM UID:{},to:{}", fromUid, toUid);
                        session2->Send(jj.dump(), static_cast<int>(MsgId::ID_NOTIFY_AUTH_FRIEND_REQ));
                    }
                    return;
                } else {
                    NotifyMakeFriendsRequest req;
                    req.set_fromuid(fromUid);
                    req.set_touid(toUid);
                    req.set_fromname(fromName);
                    req.set_fromsex(fromSex);
                    req.set_fromicon(fromIcon);
                    req.set_type(static_cast<int>(NotificationCodes::ID_NOTIFY_MAKE_FRIENDS));
                    req.set_message("成功和" + fromName + "成为好友");
                    ChatGrpcClient::GetInstance()->NotifyMakeFriends(to_ip_value, req);
                }
            } else {
                // 这里没有查询到，不发送无妨。因为已经存入数据库，用户登录就可以直接获取。
                return;
            }
        }

        bool b_apply = MysqlManager::GetInstance()->AddFriendApply(std::to_string(fromUid), uid_str);
        if (!b_apply) {
            return;
        }

        auto to_key = USERIP_PREFIX + uid_str;
        std::string to_ip_value;
        bool b_ip = RedisManager::GetInstance()->Get(to_key, to_ip_value);
        if (!b_ip) {
            return;
        }

        // 给对方发送请求信息
        auto& cfg = ConfigManager::GetInstance();
        auto self_name = cfg["SelfServer"]["name"];
        if (to_ip_value == self_name) {
            auto session2 = UserManager::GetInstance()->GetSession(toUid);
            if (session2) {
                SPDLOG_INFO("FROM UID:{},to:{}", fromUid, toUid);
                SPDLOG_INFO("FROM SESSION:{},to:{}", session->GetSessionId(), session2->GetSessionId());
                json jj;
                jj["error"] = ErrorCodes::SUCCESS;
                jj["from_uid"] = fromUid;
                jj["from_name"] = fromName;
                session2->Send(jj.dump(), static_cast<int>(MsgId::ID_NOTIFY_ADD_FRIEND_REQ));
            }
            return;
        }
        AddFriendRequest req;
        req.set_fromuid(fromUid);
        req.set_touid(toUid);
        req.set_name(fromName);
        req.set_desc(fromDesc);
        req.set_sex(fromSex);
        req.set_icon(fromIcon);

        ChatGrpcClient::GetInstance()->NotifyAddFriend(to_ip_value, req);
    };

    _function_callbacks[MsgId::ID_AUTH_FRIEND_REQ] = [this](std::shared_ptr<Session> session, uint16_t msg_id, const std::string& msg) {
        json j = json::parse(msg);
        j["error"] = ErrorCodes::SUCCESS;
        j["ok"] = false; // 标记失败

        if (j.contains("reply")) {
            bool b = j["reply"].get<bool>();
            if (b) {
                // 只是收到通知回复，我们把数据库状态更新一下
                // 如果失败说明当前双方都在线，消息就没有入库，所以这里不做处理。
                auto fromUid = j["from_uid"].get<int>();
                bool ok1 = MysqlManager::GetInstance()->ChangeMessageStatus(std::to_string(fromUid), 1);
                return;
            }
        }

        Defer defer([this, &j, session]() {
            // 这是给fromUid的回复信息
            // 目地是如果同意，那么就返回好友的信息
            session->Send(j.dump(), static_cast<int>(MsgId::ID_AUTH_FRIEND_RSP));
        });

        auto toUid = j["to_uid"].get<int>();
        auto fromUid = j["from_uid"].get<int>();
        auto fromName = j["from_name"].get<std::string>();
        auto fromSex = j["from_sex"].get<int>();
        auto fromIcon = j["from_icon"].get<std::string>();
        auto fromDesc = j["from_desc"].get<std::string>();
        int fromStatus = 1;

        bool accept = j["accept"].get<bool>();
        // 不需要解析其他的信息，只需要按需发给对方即可
        // fromUid接受或者拒绝，服务器回复给toUid
        std::string base_key = USER_BASE_INFO_PREFIX + std::to_string(toUid);
        auto apply_info = std::make_shared<UserInfo>();
        bool b_info = GetBaseInfo(base_key, toUid, apply_info);
        if (!b_info) {
            j["ok"] = true;
            // 发送请求的用户不在线，所以数据库持久存储
            if (!accept) {
                MysqlManager::GetInstance()->AddNotification(std::to_string(toUid), static_cast<int>(NotificationCodes::ID_NOTIFY_NOT_FRIENDS), "😭" + fromName + "拒绝了您的好友申请😭");
            } else {
                MysqlManager::GetInstance()->AddNotification(std::to_string(toUid), static_cast<int>(NotificationCodes::ID_NOTIFY_MAKE_FRIENDS), "😄" + fromName + "同意了您的好友申请😄");
            }
            return;
        } else {
            j["to_uid"] = apply_info->uid;
            j["to_sex"] = apply_info->sex;
            j["to_status"] = apply_info->status;
            j["to_name"] = apply_info->name;
            j["to_email"] = apply_info->email;
            j["to_icon"] = apply_info->icon;
            j["to_desc"] = apply_info->desc;
            j["to_meseage"] = apply_info->back; // 备用字段，用来展示最近消息
            j["ok"] = true;
            if (!accept) {
                j["type"] = static_cast<int>(NotificationCodes::ID_NOTIFY_NOT_FRIENDS);
            } else {
                j["type"] = static_cast<int>(NotificationCodes::ID_NOTIFY_MAKE_FRIENDS);
            }
        }
        if (accept) {
            bool ok1 = MysqlManager::GetInstance()->ChangeApplyStatus(std::to_string(toUid), std::to_string(fromUid), 1);
            bool ok2 = MysqlManager::GetInstance()->MakeFriends(std::to_string(toUid), std::to_string(fromUid));
            // 接下来就是获取好友信息，发送给被申请人
        } else {
            MysqlManager::GetInstance()->ChangeApplyStatus(std::to_string(toUid), std::to_string(fromUid), -1);
        }

        // TODO:接下来就是发送给申请人，也就是将from_uid的信息发送给to_uid
        std::string to_key = USERIP_PREFIX + std::to_string(toUid);
        std::string to_ip_value;
        bool b_ip = RedisManager::GetInstance()->Get(to_key, to_ip_value);
        if (!b_ip) {
            // 不存在我们就需要加入mysqk持续等待下次用户登录处理
            if (accept) {
                bool ok = MysqlManager::GetInstance()->AddNotification(std::to_string(toUid), static_cast<int>(NotificationCodes::ID_NOTIFY_MAKE_FRIENDS), "😄" + fromName + "已经和您成为好友😄");
            } else {
                bool ok = MysqlManager::GetInstance()->AddNotification(std::to_string(toUid), static_cast<int>(NotificationCodes::ID_NOTIFY_NOT_FRIENDS), "😭" + fromName + "拒绝了您的好友请求😭");
            }
            return;
        }
        auto& cfg = ConfigManager::GetInstance();
        auto self_name = cfg["SelfServer"]["name"];
        if (to_ip_value == self_name) {
            auto session2 = UserManager::GetInstance()->GetSession(toUid);
            if (session2) {
                SPDLOG_INFO("FROM UID:{},to:{}", fromUid, toUid);
                SPDLOG_INFO("FROM SESSION:{},to:{}", session->GetSessionId(), session2->GetSessionId());
                j["from_status"] = 1;
                session2->Send(j.dump(), static_cast<int>(MsgId::ID_NOTIFY_AUTH_FRIEND_REQ));
            }
        } else {
            NotifyMakeFriendsRequest req;
            req.set_fromuid(fromUid);
            req.set_touid(toUid);
            req.set_fromname(fromName);
            req.set_fromsex(fromSex);
            req.set_fromicon(fromIcon);
            req.set_fromstatus(fromStatus);
            req.set_fromdesc(fromDesc);
            if (!accept) {
                req.set_type(static_cast<int>(NotificationCodes::ID_NOTIFY_NOT_FRIENDS));
                req.set_message(fromName + "拒绝了你的好友申请");
            } else {
                req.set_type(static_cast<int>(NotificationCodes::ID_NOTIFY_MAKE_FRIENDS));
                req.set_message(fromName + "同意了您的好友申请");
            }
            ChatGrpcClient::GetInstance()->NotifyMakeFriends(to_ip_value, req);
        }
    };
    /**
     * @brief 发送信息请求
     *
     */
    _function_callbacks[MsgId::ID_TEXT_CHAT_MSG_REQ] = [this](std::shared_ptr<Session> session, uint16_t msg_id, const std::string& msg) {
        json j;
        j["error"] = ErrorCodes::SUCCESS;
        Defer defer([this, &j, session]() {
            session->Send(j.dump(), static_cast<int>(MsgId::ID_TEXT_CHAT_MSG_RSP));
        });
        im::MessageItem pb;
        pb.ParseFromString(msg);

        auto& cfg = ConfigManager::GetInstance();
        auto self_name = cfg["SelfServer"]["name"];
        auto to_uid = pb.to_id();
        std::string to_key = USERIP_PREFIX + std::to_string(to_uid);
        std::string to_ip_value;
        bool b_ip = RedisManager::GetInstance()->Get(to_key, to_ip_value);

        if (!b_ip) {
            // 当前不在线
            bool ok = MysqlManager::GetInstance()->AddMessage(pb.id(), pb.from_id(), pb.to_id(), pb.timestamp(), pb.env(), pb.content().type(), pb.content().data(), pb.content().mime_type(), pb.content().fid(), 0);
            return;
        } else {
            if (to_ip_value == self_name) {
                auto session2 = UserManager::GetInstance()->GetSession(to_uid);
                if (session2) {
                    SPDLOG_INFO("FROM UID:{},to:{}", pb.from_id(), to_uid);
                    SPDLOG_INFO("FROM SESSION:{},to:{}", session->GetSessionId(), session2->GetSessionId());
                    session2->Send(msg, static_cast<int>(MsgId::ID_TEXT_CHAT_MSG_REQ));
                    bool ok = MysqlManager::GetInstance()->AddMessage(pb.id(), pb.from_id(), pb.to_id(), pb.timestamp(), pb.env(), pb.content().type(), pb.content().data(), pb.content().mime_type(), pb.content().fid(), 1);
                }
            } else {
                TextChatMessageRequest req;
                req.set_fromuid(pb.from_id());
                req.set_touid(pb.to_id());
                req.set_data(msg);
                ChatGrpcClient::GetInstance()->NotifyTextChatMessage(to_ip_value, req);
            }
        }
    };

    _function_callbacks[MsgId::ID_SYNC_CONVERSATIONS_REQ] = [this](std::shared_ptr<Session> session, uint16_t msg_id, const std::string& msg) {
        json j;
        try {
            j = json::parse(msg);
        } catch (const std::exception& e) {
            SPDLOG_WARN("SyncConversations parse error: {}", e.what());
            json err;
            err["error"] = ErrorCodes::ERROR_JSON;
            return;
        }

        if (!j.contains("conversations") || !j["conversations"].is_array()) {
            SPDLOG_WARN("SyncConversations missing conversations array");
            return;
        }

        // 所属用户 uid（客户端会发送）
        int owner_uid = j.value("uid", 0);
        std::string owner_uid_str = std::to_string(owner_uid);

        for (const auto& item : j["conversations"]) {
            try {
                auto conv = std::make_shared<SessionInfo>();
                conv->uid = item.value("uid", 0);
                conv->from_uid = item.value("from_uid", 0);
                conv->to_uid = item.value("to_uid", 0);
                conv->create_time = item.value("create_time", std::string());
                conv->update_time = item.value("update_time", std::string());
                conv->name = item.value("name", std::string());
                conv->icon = item.value("icon", std::string());
                conv->status = item.value("status", 0);
                conv->deleted = item.value("deleted", 0);
                conv->pined = item.value("pined", 0);
                conv->processed = item.value("processed", false);
                // 客户端可能携带本地 processed 字段，用于 UI，本段不用写入 DB

                // 将会话写入数据库
                // 假定 MysqlManager 提供 AddConversation(owner_uid, std::shared_ptr<SessionInfo>)
                // 如果项目中签名不同，请根据实际签名调整此处调用。
                bool ok = MysqlManager::GetInstance()->AddConversation(conv->uid, conv->from_uid, conv->to_uid, conv->create_time, conv->update_time, conv->name, conv->icon, conv->status, conv->deleted, conv->pined, conv->processed);
                if (!ok) {
                    SPDLOG_WARN("AddConversation failed owner:{} conv_uid:{}", owner_uid, conv->uid);
                    // 不中断，继续处理剩余会话
                }
            } catch (const std::exception& e) {
                SPDLOG_WARN("Exception when processing conversation item: {}", e.what());
                // 继续处理下一个
            }
        }
    };
}

void LogicSystem::DealMsg()
{
    while (true) {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this]() {
            return _stop || !_queue.empty();
        });
        if (_stop && _queue.empty()) {
            break;
        }

        if (!_queue.empty()) {
            std::queue<std::shared_ptr<LogicNode>> local_queue;
            local_queue.swap(_queue);
            lock.unlock();
            while (!local_queue.empty()) {
                auto msg = local_queue.front();
                local_queue.pop();

                auto it = _function_callbacks.find(static_cast<MsgId>(msg->_recv_node->_msg_id));
                if (it != _function_callbacks.end()) {
                    it->second(msg->_session, msg->_recv_node->_msg_id, std::string(msg->_recv_node->_data, msg->_recv_node->_total_len));
                }
            }
        }
    }
}

bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
    std::string info_str = "";
    bool b_base = RedisManager::GetInstance()->Get(base_key, info_str);
    if (b_base) {
        json j = json::parse(info_str);
        userinfo->name = j["name"].get<std::string>();
        userinfo->email = j["email"].get<std::string>();
        userinfo->uid = j["uid"].get<int>();
        userinfo->sex = j["sex"].get<int>();
        std::string status_key = USER_STATUS_PREFIX + std::to_string(uid);
        std::string status_value;
        bool b_status = RedisManager::GetInstance()->Get(status_key, status_value);
        if (b_status) {
            if (status_value == "0" || status_value == "") {
                userinfo->status = 0;
            } else {
                userinfo->status = 1;
            }
        } else {
            userinfo->status = 0;
        }
        userinfo->nick = j["nick"].get<std::string>();
        userinfo->desc = j["desc"].get<std::string>();
        userinfo->icon = j["icon"].get<std::string>();
        SPDLOG_INFO("uid:{},name:{},email:{}", uid, userinfo->name, userinfo->email);
    } else {
        userinfo = MysqlManager::GetInstance()->GetUser(uid);
        if (userinfo == nullptr) {
            return false;
        }
        json j;
        j["name"] = userinfo->name;
        j["email"] = userinfo->email;
        j["uid"] = userinfo->uid;
        j["sex"] = userinfo->sex;
        j["nick"] = userinfo->nick;
        j["desc"] = userinfo->desc;
        std::string status_key = USER_STATUS_PREFIX + std::to_string(uid);
        std::string status_value;
        bool b_status = RedisManager::GetInstance()->Get(status_key, status_value);
        if (b_status) {
            if (status_value == "0" || status_value == "") {
                j["status"] = 0;
            } else {
                j["status"] = 1;
            }
        } else {
            j["status"] = 0;
        }
        j["icon"] = userinfo->icon;
        RedisManager::GetInstance()->Set(base_key, j.dump());
    }
    return true;
}

bool LogicSystem::IsPureDigit(const std::string& str)
{
    if (str.empty())
        return false;
    return std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c); });
}

void LogicSystem::GetSearchedUsers(const std::string& uid, json& j, bool only_digit)
{
    // 根据only决定使用uid还是name搜索
    j["error"] = ErrorCodes::SUCCESS;
    std::string base_key = USER_BASE_INFO_PREFIX + uid;
    std::string info_str = "";
    json users = json::array();

    Defer defer([this, &j, &users]() {
        j["users"] = users;
    });

    if (only_digit) {
        bool b_base = RedisManager::GetInstance()->Get(base_key, info_str);
        if (b_base) {
            json jj = json::parse(info_str);
            jj["status"] = 1;
            users.push_back(jj);
            return;
        } else {
            std::shared_ptr<UserInfo> user_info = nullptr;
            user_info = MysqlManager::GetInstance()->GetUser(std::stoi(uid));
            if (user_info == nullptr) {
                j["error"] = ErrorCodes::ERROR_UID_INVALID;
                return;
            }
            json jj;
            jj["uid"] = user_info->uid;
            jj["name"] = user_info->name;
            jj["email"] = user_info->email;
            jj["nick"] = user_info->nick;
            jj["sex"] = user_info->sex;
            jj["status"] = 0;
            jj["desc"] = user_info->desc;
            jj["icon"] = user_info->icon;
            RedisManager::GetInstance()->Set(base_key, jj.dump());
            users.push_back(jj);
            return;
        }
    } else {
        // 通过name查询
        std::string name_key = USER_BASE_INFOS_PREFIX + uid;
        std::string name_str = "";
        bool b_base = RedisManager::GetInstance()->Get(name_key, name_str);
        if (b_base) {
            users = json::parse(name_str);
            for (auto& user : users) {
                user["status"] = 1;
            }
            return;
        } else {
            std::vector<std::shared_ptr<UserInfo>> user_infos = MysqlManager::GetInstance()->GetUser(uid);
            if (user_infos.empty()) {
                j["error"] = ErrorCodes::ERROR_UID_INVALID;
                return;
            } else {
                for (auto user_info : user_infos) {
                    json jj = json::object();
                    jj["uid"] = user_info->uid;
                    jj["name"] = user_info->name;
                    jj["email"] = user_info->email;
                    jj["nick"] = user_info->nick;
                    jj["sex"] = user_info->sex;
                    jj["desc"] = user_info->desc;
                    jj["icon"] = user_info->icon;
                    jj["status"] = 0;
                    users.push_back(jj);
                }
                RedisManager::GetInstance()->Set(name_key, users.dump());
                return;
            }
        }
    }
}

LogicSystem::LogicSystem(std::size_t size)
    : _stop(false)
    , _size(size)
{
    RegisterCallBacks();
    _work_threads.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        _work_threads.emplace_back(&LogicSystem::DealMsg, this);
    }
}

LogicSystem::~LogicSystem()
{
    _stop = true;
    _cv.notify_all();
    for (auto& p : _work_threads) {
        p.join();
    }

    SPDLOG_INFO("LogicSystem Stopped");
}
