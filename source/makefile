.PHONY: gobang
gobang:gobang.cc util.hpp logger.hpp db.hpp online.hpp room.hpp
	g++ -std=c++11 $^ -o $@ -L/usr/lib64/mysql -lmysqlclient -lpthread -lboost_system -ljsoncpp