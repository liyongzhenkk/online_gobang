#ifndef _M_ONLINE_H__
#define _M_ONLINE_H__
#include "util.hpp"
#include <unordered_map>
#include <mutex>
class online_manager
{
private:
    std::mutex _mutex;
    std::unordered_map<uint64_t, wsserver_t::connection_ptr> _hall_user;
    std::unordered_map<uint64_t, wsserver_t::connection_ptr> _room_user;

public:
    bool enter_game_hall(uint64_t uid, wsserver_t::connection_ptr &conn)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _hall_user.insert(std::make_pair(uid, conn));
    }
    bool enter_game_room(uint64_t uid, wsserver_t::connection_ptr &conn)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _room_user.insert(std::make_pair(uid, conn));
    }
    void exit_game_hall(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _hall_user.erase(uid);
    }
    void exit_game_room(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _room_user.erase(uid);
    }
    bool is_in_game_hall(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _hall_user.find(uid);
        if (it == _hall_user.end())
        {
            return false;
        }
        return true;
    }
    bool is_in_game_room(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _room_user.find(uid);
        if (it == _room_user.end())
        {
            return false;
        }
        return true;
    }
    wsserver_t::connection_ptr get_conn_from_hall(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _room_user.find(uid);
        if (it == _room_user.end())
        {
            return wsserver_t::connection_ptr();
        }
        return it->second;
    }
    wsserver_t::connection_ptr get_conn_from_room(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _room_user.find(uid);
        if (it == _room_user.end())
        {
            return wsserver_t::connection_ptr();
        }
        return it->second;
    }
};

#endif