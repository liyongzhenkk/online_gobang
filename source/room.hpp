#ifndef _M_ROOM_H__
#define _M_ROOM_H__ 0

#include "util.hpp"
#include "logger.hpp"
#include "db.hpp"
#include "online.hpp"
#define BOARD_ROW 15
#define BOARD_COL 15
#define CHESS_WHITE 1
#define CHESS_BLACK 2

typedef enum
{
    GAME_START,
    GAME_OVER
} room_statu;
class room
{
private:
    uint64_t _room_id;
    room_statu _statu;
    int _player_count;
    uint64_t _white_id;
    uint64_t _black_id;
    user_table *_tb_user;
    online_manager *_online_user;
    std::vector<std::vector<int>> _board;

private:
    bool five(int row, int col, int row_off, int col_off, int color)
    {
        // row和col是下棋位置，  row_off和col_off是偏移量，也是方向
        int count = 1;
        int search_row = row + row_off;
        int search_col = col + col_off;
        while (search_row >= 0 && search_row < BOARD_ROW &&
               search_col >= 0 && search_col < BOARD_COL &&
               _board[search_row][search_col] == color)
        {
            // 同色棋子数量++
            count++;
            // 检索位置继续向后偏移
            search_row += row_off;
            search_col += col_off;
        }
        search_row = row - row_off;
        search_col = col - col_off;
        while (search_row >= 0 && search_row < BOARD_ROW &&
               search_col >= 0 && search_col < BOARD_COL &&
               _board[search_row][search_col] == color)
        {
            // 同色棋子数量++
            count++;
            // 检索位置继续向后偏移
            search_row -= row_off;
            search_col -= col_off;
        }
        return (count >= 5);
    }
    uint64_t check_win(int row, int col, int color)
    {
        if (five(row, col, 0, 1, color) ||
            five(row, col, 1, 0, color) ||
            five(row, col, -1, 1, color) ||
            five(row, col, -1, -1, color))
        {
            // 任意一个方向上出现了true也就是五星连珠，则设置返回值
            return color == CHESS_WHITE ? _white_id : _black_id;
        }
        return 0;
    }

public:
    room(uint64_t room_id, user_table *tb_user, online_manager *online_user)
        : _room_id(room_id), _statu(GAME_START), _player_count(0),
          _tb_user(tb_user), _online_user(_online_user), _board(BOARD_ROW, std::vector<int>(BOARD_COL, 0))
    {
        DLOG("%lu 房间创建成功！！ ", _room_id);
    }
    ~room()
    {
        DLOG("%lu 房间销毁成功！！ ", _room_id);
    }
    uint64_t id()
    {
        return _room_id;
    }
    room_statu statu()
    {
        return _statu;
    }
    int player_count()
    {
        return _player_count;
    }
    void add_write_user(uint64_t uid)
    {
        _white_id = uid;
    }
    void add_black_user(uint64_t uid)
    {
        _black_id = uid;
    }
    uint64_t get_white_user()
    {
        return _white_id;
    }
    uint64_t get_black_user()
    {
        return _black_id;
    }

    // 下棋
    Json::Value handle_chess(Json::Value &req)
    {
        Json::Value json_resp;
        // 1.
        // uint64_t room_id = req["room_id"].asUInt64();
        // if (room_id != _room_id)
        // {
        //     json_resp["optype"] = "put_chess";
        //     json_resp["result"] = false;
        //     json_resp["reason"] = "房间号不匹配！";
        //     return json_resp;
        // }

        // 2.
        int chess_row = req["row"].asInt();
        int chess_col = req["col"].asInt();
        uint64_t cur_uid = req["uid"].asUInt64();
        if (_online_user->is_in_game_room(_white_id) == false)
        {
            json_resp["optype"] = "put_chees";
            json_resp["result"] = true;
            json_resp["reason"] = "对方掉线，不战而胜！";
            json_resp["room_id"] = _room_id;
            json_resp["uid"] = cur_uid;
            json_resp["row"] = chess_row;
            json_resp["col"] = chess_col;
            json_resp["winner"] = _black_id;

            // 更新数据库用户信息
            _tb_user->win(_black_id);
            _tb_user->lose(_white_id);

            return json_resp;
        }
        if (_online_user->is_in_game_room(_black_id) == false)
        {
            json_resp["optype"] = "put_chees";
            json_resp["result"] = true;
            json_resp["reason"] = "对方掉线，不战而胜！";
            json_resp["room_id"] = _room_id;
            json_resp["uid"] = cur_uid;
            json_resp["row"] = chess_row;
            json_resp["col"] = chess_col;
            json_resp["winner"] = _white_id;

            // 更新数据库用户信息
            // _tb_user->win(_white_id);
            // _tb_user->lose(_black_id);

            return json_resp;
        }

        // 3.
        if (_board[chess_row][chess_col] != 0)
        {
            json_resp["optype"] = "put_chess";
            json_resp["result"] = false;
            json_resp["reason"] = "当前位置已有其他棋子！";
            return json_resp;
        }
        int cur_color = cur_uid == _white_id ? CHESS_WHITE : CHESS_BLACK;
        _board[chess_row][chess_col] = cur_color;

        // 4.
        uint64_t winner_id = check_win(chess_row, chess_col, cur_color);
        if (winner_id != 0)
        {
            json_resp["reason"] = "五星联珠，战无敌！";
            // 更新数据库用户信息
            // uint64_t loser_id = winner_id == _white_id ? _black_id : _white_id;
            // _tb_user->win(winner_id);
            // _tb_user->lose(loser_id);
        }

        json_resp["optype"] = "put_chees";
        json_resp["result"] = true;
        // json_resp["reason"] = "五星联珠，战无敌！";
        //  json_resp["room_id"] = _room_id;
        //  json_resp["uid"] = cur_uid;
        //  json_resp["row"] = chess_row;
        //  json_resp["col"] = chess_col;
        json_resp["winner"] = winner_id;
        return json_resp;
    }

    // 聊天
    Json::Value handle_chat(Json::Value &req)
    {
        Json::Value json_resp = req;
        // 检查房间是否一致
        // uint64_t room_id = req["room_id"].asInt64();
        // if (room_id != _room_id)
        // {
        //     json_resp["result"] = false;
        //     json_resp["reason"] = "房间号不匹配！";
        //     return json_resp;
        // }
        // 检测消息是否含有敏感词
        std::string msg = req["mseeage"].asString();
        size_t pos = msg.find("垃圾");
        if (pos != std::string::npos)
        {
            json_resp["result"] = false;
            json_resp["reason"] = "消息包含敏感词！";
            return json_resp;
        }

        // 广播消息-返回消息
        json_resp["result"] = true;
        return json_resp;
    }
    // 玩家推出
    void handle_exit(uint64_t uid)
    {
        // 如果是下棋当中退出，则对方胜利； 如果是下棋结束了，则是正常退出
        Json::Value json_resp;
        if (_statu == GAME_START)
        {
            json_resp["optype"] = "put_chees";
            json_resp["result"] = true;
            json_resp["reason"] = "对方掉线，不战而胜！";
            json_resp["room_id"] = _room_id;
            json_resp["uid"] = uid;
            json_resp["row"] = -1;
            json_resp["col"] = -1;
            json_resp["winner"] = uid == _white_id ? _black_id : _white_id;
        }
        broadcast(json_resp);
        // 房间玩家数量--
        _player_count--;
        return;
    }
    // 总的请求处理函数，在函数内部根据请求，调用不同的处理函数
    void handle_request(Json::Value req)
    {
        // 1.检查房间号是否匹配
        Json::Value json_resp;
        uint64_t room_id = req["room_id"].asUInt64();
        if (room_id != _room_id)
        {
            json_resp["optype"] = req["optype"].asString();
            json_resp["result"] = false;
            json_resp["reason"] = "房间号不匹配！";
            return broadcast(json_resp);
        }

        // 2.根据不同的请求类型 调用不同的处理函数

        if (req["optype"].asString() == "put_chess")
        {
            json_resp = handle_chess(req);
            if (json_resp["winner"].asInt64() != 0)
            {
                uint64_t winner_id = json_resp["winner"].asInt64();
                uint64_t loser_id = winner_id == _white_id ? _black_id : _white_id;
                _tb_user->win(winner_id);
                _tb_user->lose(loser_id);
                _statu = GAME_OVER;
            }
        }
        else if (req["optype"].asString() == "chat")
        {
            json_resp = handle_chat(req);
        }
        else
        {
            json_resp["optype"] = req["optype"].asString();
            json_resp["result"] = false;
            json_resp["reason"] = "未知请求错误！";
        }

        return broadcast(json_resp);
    }
    // 广播
    void broadcast(Json::Value &req)
    {
        // 1.对要响应的信息进行序列话，将json value 序列话成字符串
        std::string body;
        Json_util::serialize(req, body);
        // 2. 获取房间中所有用户的一个链接
        // 3. 逐个进行发送相应信息
        wsserver_t::connection_ptr wconn = _online_user->get_conn_from_room(_white_id);
        if (wconn.get() != nullptr)
        {
            wconn->send(body);
        }
        wsserver_t::connection_ptr bconn = _online_user->get_conn_from_room(_black_id);
        if (bconn.get() != nullptr)
        {
            bconn->send(body);
        }
        return;
    }
};

using room_ptr = std::shared_ptr<room>;

class room_manager
{
private:
    uint64_t _next_rid;
    std::mutex _mutex;
    user_table *_tb_user;
    online_manager *_online_user;
    std::unordered_map<uint64_t, room_ptr> _rooms;
    std::unordered_map<uint64_t, uint64_t> _users;

public:
    //初始化房间ID计数器
    room_manager(user_table* ut, online_manager* om)
    :_next_rid(1),_tb_user(ut),_online_user(om)
    {
        DLOG("房间管理模块初始化完成！");
    }
    ~room_manager()
    {
        DLOG("房间管理模块即将销毁！");
    }
    //为两个用户创建房间，并返回房间的智能指针管理对象
    room_ptr create_room(uint64_t uid1, uint64_t uid2)
    {
        //两个用户在游戏大厅中进行对战匹配，匹配成功后创建房间
        //1.检验两个用户是否还在游戏大厅中，只有都在才需要创建房间
        if(_online_user->is_in_game_hall(uid1) == false)
        {
            DLOG("用户: %lu 不在大厅中，创建房间失败!! ",uid1);
            return room_ptr();
        }
        if(_online_user->is_in_game_hall(uid2) == false)
        {
            DLOG("用户: %lu 不在大厅中，创建房间失败!! ",uid2);
            return room_ptr();
        }
        //2.创建房间，将用户信息添加到房间中
        std::unique_lock<std::mutex> lock(_mutex);
        room_ptr rp(new room(_next_rid,_tb_user,_online_user));
        rp->add_write_user(uid1);
        rp->add_black_user(uid2);
        //3.将房间信息管理起来
        _rooms.insert(std::make_pair(_next_rid,rp));
        _users.insert(std::make_pair(uid1,_next_rid));
        _users.insert(std::make_pair(uid2,_next_rid));
        _next_rid++;
        //4.返回房间信息
        return rp;
    }
    //通过房间ID获取房间信息
    room_ptr get_room_by_rid(uint64_t rid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _rooms.find(rid);
        if(it == _rooms.end())
        {
            return room_ptr();
        }
        return it->second;
    }
    //通过用户ID获取房间信息
    room_ptr get_room_by_uid(uint64_t uid)
    {
        //1.通过用户id 查找房间id
        std::unique_lock<std::mutex> lock(_mutex);
        auto uit = _users.find(uid);
        if(uit == _users.end())
        {
            return room_ptr();
        }
        //2.通过房间id 获取房间信息

        //！！！！！//锁冲突 不可重入死锁 ！！！！！
        //return get_room_by_rid(it->second);
        uint64_t rid = uit->second;
        auto rit = _rooms.find(rid);
        if(rit == _rooms.end())
        {
            return room_ptr();
        }
        return rit->second;
    }
    //通过房间ID销毁房间
    void remove_room(uint64_t rid)
    {
        //因为房间信息是通过shared_ptr 在 rooms中管理，因此只要将shared_ptr从rooms中移除
        //则sharedptr计数器 == 0 ，外界没有对房间信息进行操作保存的情况下就会释放
        //1.通过房间id 找到房间信息
        room_ptr rp = get_room_by_rid(rid);
        if(rp.get() == nullptr)
        {
            return;
        }
        //2.通过房间信息，获取房间中所有用户的id
        uint64_t uid1 = rp->get_white_user();
        uint64_t uid2 = rp->get_black_user();
        //3.移除房间管理中的用户信息
        std::unique_lock<std::mutex> lock(_mutex);
        _users.erase(uid1);
        _users.erase(uid2);
        //4.移除房间管理信息
        _rooms.erase(rid);
    }
    //删除房间中指定用户，如果房间中没有用户，则销毁房间
    void remove_room_user(uint64_t uid)
    {
        room_ptr rp = get_room_by_uid(uid);
        if(rp.get() == nullptr){
            return ;
        }
        //处理房间中玩家退出动作
        rp->handle_exit(uid);
        //房间中没有玩家，销毁房间
        if(rp->player_count() == 0)
        {
            remove_room(rp->id());
        }
        return ;
    }
};

#endif