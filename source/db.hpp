#ifndef __M_DB_H__
#define __M_DB_H__
#include "util.hpp"
#include <assert.h>
#include<mutex>

class user_table
{
private:
    MYSQL *_mysql;
    std::mutex _mutex;

public:
    user_table(const std::string &host,
               const std::string &username,
               const std::string &password,
               const std::string &dbname,
               uint16_t port = 3306)
    {
        _mysql = mysql_util::mysql_create(host, username, password, dbname, port);
        assert(_mysql != NULL);
    }
    ~user_table()
    {
        mysql_util::mysql_destory(_mysql);
        _mysql = NULL;
    }
    bool insert(Json::Value &user)
    {
#define INSERT_USER "insert user values(null, '%s', password('%s'), 1000, 0, 0);"


        // Json::Value val;
        // bool ret = select_by_name(user["username"].asCString(), val);
        // if (ret == true)
        // {
        //     DLOG("user: %s is already exists", user["username"].asCString());
        //     return false;
        // }
        char sql[4096] = {0};
        sprintf(sql, INSERT_USER, user["username"].asCString(), user["password"].asCString());
        bool ret = mysql_util::mysql_exec(_mysql, sql);
        if (ret == false)
        {
            ELOG("insert user info faied!!\n");
            return false;
        }
        return true;
    }
    bool login(Json::Value &user)
    {
#define LOGIN_USER "select id, score,total_count,win_count from user where username=`%s` and password=password(`%s`);"
        char sql[4096] = {0};
        sprintf(sql, LOGIN_USER, user["username"].asCString(), user["password"].asCString());
        MYSQL_RES *res = NULL;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            bool ret = mysql_util::mysql_exec(_mysql, sql);
            if (ret == false)
            {
                DLOG("user login failed!!\n");
                return false;
            }

            res = mysql_store_result(_mysql);
            if (res == NULL)
            {
                DLOG("have no login user info!!\n");
                return false;
            }
        }
        int row_num = mysql_num_rows(res);
        if (row_num != 1)
        {
            DLOG("The user information queried is not unique!!\n");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = std::stol(row[0]);
        user["score"] = std::stol(row[1]);
        user["total_count"] = std::stol(row[2]);
        user["win_count"] = std::stol(row[3]);
        mysql_free_result(res);
        return true;
    }
    bool select_by_name(const std::string &name, Json::Value &user)
    {
#define USER_BY_NAME "select id,score,total_count,win_count from user where username='%s';"
        char sql[4096] = {0};
        sprintf(sql, USER_BY_NAME, name.c_str());
        MYSQL_RES *res = NULL;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            bool ret = mysql_util::mysql_exec(_mysql, sql);
            if (ret == false)
            {
                DLOG("have no login user info!!");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == NULL)
            {
                DLOG("have no user info!!");
                return false;
            }
        }
        int row_num = mysql_num_rows(res);
        if (row_num != 1)
        {
            DLOG("the user information queried is not unique!!");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = std::stol(row[0]);
        user["username"] = name;
        user["score"] = std::stol(row[1]);
        user["total_count"] = std::stol(row[2]);
        user["win_count"] = std::stol(row[3]);
        mysql_free_result(res);
        return true;
    }
    bool select_by_id(uint64_t id, Json::Value &user)
    {
#define USER_BY_ID "select username,score,total_count,win_count from user where id=%d;"
        char sql[4096] = {0};
        sprintf(sql, USER_BY_ID, id);
        MYSQL_RES *res = NULL;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            bool ret = mysql_util::mysql_exec(_mysql, sql);
            if (ret == false)
            {
                DLOG("have no login user info!!");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == NULL)
            {
                DLOG("have no user info!!");
                return false;
            }
        }
        int row_num = mysql_num_rows(res);
        if (row_num != 1)
        {
            DLOG("the user information queried is not unique!!");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = id;
        user["username"] = row[0];
        user["score"] = std::stol(row[1]);
        user["total_count"] = std::stol(row[2]);
        user["win_count"] = std::stol(row[3]);
        mysql_free_result(res);
        return true;
    }
    bool win(uint64_t id)
    {
#define USER_WIN "update user set score=score+30 total_count=total_count+1 win_count=win_count+1 where id=%d;"
        char sql[4096] = {0};
        sprintf(sql, USER_WIN, id);
        bool ret = mysql_util::mysql_exec(_mysql, sql);
        if (ret == false)
        {
            DLOG("update win user info failed!!\n");
            return false;
        }
        return true;
    }
    bool lose(uint64_t id)
    {
#define USER_LOSE "update user set score=score-30 total_count=total_count+1 where id=%d;"
        char sql[4096] = {0};
        sprintf(sql, USER_LOSE, id);
        bool ret = mysql_util::mysql_exec(_mysql, sql);
        if (ret == false)
        {
            DLOG("update lose user info failed!!\n");
            return false;
        }
        return true;
    }
};

#endif