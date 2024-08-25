#include"util.hpp"
#include"db.hpp"
#include"online.hpp"
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
    room();
    ~room();
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