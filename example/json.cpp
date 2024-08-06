#include <iostream>
#include <vector>
#include <string>
#include <jsoncpp/json/json.h>
// 使用json

std::string serialize()
{
    // 1. 将我们需要序列化的数据 存储到 Json Value对象中
    Json::Value root;
    root["姓名"] = "小明";
    root["年龄"] = 18;
    root["成绩"].append(98);
    root["成绩"].append(88.5);
    root["成绩"].append(78.5);
    // 2. 实例化一个streamwriterbuilder 工厂类对象
    Json::StreamWriterBuilder swb;
    // 3.通过streamwriterbuilder 工厂类对象 生产一个streamwriter 对象
    Json::StreamWriter *sw = swb.newStreamWriter();
    // 4. 使用 streamwrite 对象 ，对Json Value中存储的数据进行序列化
    std::stringstream ss;
    int ret = sw->write(root, &ss);
    if (ret != 0)
    {
        std::cout << "json serialize failed!!" << std::endl;
        return "failed!!";
    }
    std::cout << ss.str() << std::endl;
    delete sw;
    return ss.str();
}
void unserialize(const std::string &str)
{
    // 1.实例化一个charreaderbulder 工厂类
    Json::CharReaderBuilder crb;
    // 2.使用CharReaderBuilder 工厂类生产一个CharReader对象
    Json::CharReader *cr = crb.newCharReader();
    // 3.定义一个Json::Value 对象存储解析之后的数据
    Json::Value root;
    // 4.使用CharReader 对象进行json =格式化字符串str反序列化
    std::string err;
    bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), &root, &err);
    if (ret == false)
    {
        std::cout << "json unserialize failed: " << err << std::endl;
        return;
    }
    // 5.逐个元素访问
    std::cout << "name = " << root["姓名"].asString() << std::endl
              << "age = " << root["年龄"].asInt() << std::endl;
    int sz = root["成绩"].size();
    for (int i = 0; i < sz; i++)
    {
        std::cout << "score = "<<root["成绩"][i].asDouble() << std::endl;
    }
    delete cr;
}
int main()
{
    std::string ss = serialize();
    unserialize(ss);
    return 0;
}