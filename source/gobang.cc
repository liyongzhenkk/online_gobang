//#include"logger.hpp"
#include"util.hpp"
#include"db.hpp"

#define HOST "127.0.0.1"
#define PORT 3306
#define USER "root"
#define PASS "123456"
#define DBNAME "gobang"

void mysql_test()
{
    MYSQL* mysql = mysql_util::mysql_create(HOST, USER, PASS, DBNAME, PORT);
    char *sql = "insert stu values(null, '小hei', 18, 53, 68, 87);";
    mysql_util::mysql_exec(mysql,sql);

    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == NULL) {
        mysql_close(mysql);
        return ;
    }
    // 7.获取查询结果集中条数
    int num_row = mysql_num_rows(res);
    int num_col = mysql_num_fields(res);
    // 8.遍历结果集
    for (int i = 0; i < num_row; i++) {
        // MYSQL_ROW mysql_fetch_row(MYSQL *res);
        MYSQL_ROW row = mysql_fetch_row(res);
        for (int i = 0; i < num_col; i++) {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }
    // 9.释放结果集
   mysql_free_result(res);
    mysql_util::mysql_destory(mysql);
    return ;

}

void Json_test()
{
    Json::Value root;
    std::string body;
    root["姓名"] = "小明";
    root["年龄"] = 18;
    root["成绩"].append(98);
    root["成绩"].append(88.5);
    root["成绩"].append(78.5);
    Json_util::serialize(root, body);
    DLOG("%s", body.c_str());
    
    Json::Value val;
    Json_util::unserialize(body, val);
    std::cout << "姓名:" << val["姓名"].asString()  << std::endl;
    std::cout << "年龄:" << val["年龄"].asInt()  << std::endl;
    int sz = val["成绩"].size();
    for (int i = 0; i < sz; i++) {
        std::cout << "成绩: " << val["成绩"][i].asFloat() << std::endl;
    }
}

void str_test() 
{
    std::string str = ",...,,123,234,,,,,345,,,,";
    std::vector<std::string> arry;
    string_util::split(str, ",", arry);
    for (auto s : arry) {
        DLOG("%s", s.c_str());
    }
}
void file_test()
{
    std::string filename = "./makefile";
    std::string body;
    file_util::read(filename, body);

    std::cout << body << std::endl;
}

void db_test()
{
    user_table ut(HOST, USER, PASS, DBNAME, PORT);
    Json::Value user;
    user["username"] = "xiaoming";
    user["password"] = "123456";
    bool ret = ut.insert(user);
    //std::string body;
    //json_util::serialize(user, body);
    //std::cout << body << std::endl;
}

int main()
{
    db_test();
    //Json_test();
    return 0;

}