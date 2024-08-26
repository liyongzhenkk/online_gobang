#include"util.hpp"
#include"db.hpp"
#include"online.hpp"
#define BOARD_ROW 15;
#define BOARD_COl 15;
typedef enum{GAME_START, GAME_OVER }room_statu;
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
public:
    room(uint64_t room_id,user_table* tb_user,online_manager* online_user)
    :_room_id(room_id),_statu(GAME_START),_player_count(0),
    _tb_user(tb_user),_online_user(_online_user), _board(BOARD_ROW,std::vector<int>(BOARD_COL,0))
    {
        DLOG("%lu 房间创建成功！！ ",_room_id);
    }
    ~room()
    {
        DLOG("%lu 房间销毁成功！！ ",_room_id);
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


    //下棋
    Json::Value handle_chess(Json::Value& req);
    //聊天
    Json::Value handle_chat(Json::Value& req);
    //玩家推出
    void handle_exit(uint64_t uid);
    //总的请求处理函数，在函数内部根据请求，调用不同的处理函数
    void handle_request(Json::Value req);
    //广播
    void broadcast(Json::Value& req);


};