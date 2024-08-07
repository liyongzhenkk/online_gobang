mysql:mysql.c
	gcc $^ -o $@ -L/usr/lib64/mysql -lmysqlclient
json:json.cpp
	g++ -std=c++11 $^ -o $@ -lpthread -lboost_system -ljsoncpp
wsserver:wsserver.cpp
	g++ -std=c++11 $^ -o $@ -lpthread -lboost_system

