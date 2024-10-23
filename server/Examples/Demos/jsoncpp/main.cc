#include <iostream>
#include <sstream>
#include <memory>
#include <json/json.h>

bool Serialize(const Json::Value &val, std::string &dest)
{
    // 由Json::StreamWriterBuilder生产出Json::StreamWriter
    Json::StreamWriterBuilder swb;
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

    // 通过Json::StreamWrite的write()进行序列化
    std::stringstream ss;
    if (sw->write(val, &ss) != 0)
    {
        std::cout << "Json序列化失败" << std::endl;
        return false;
    }

    dest = ss.str();

    return true;
}

bool UnSerialize(const std::string &src, Json::Value &val)
{
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());

    std::string err;
    if (cr->parse(src.c_str(), src.c_str() + src.size(), &val, &err) == false)
    {
        std::cout << "json反序列化失败: " << err << std::endl;
        return false;
    }

    return true;
}

int main()
{
    char name[] = "SnowK";
    int age = 18;
    float score[3] = {100, 99, 98};

    Json::Value stu;
    stu["Name"] = name;
    stu["Age"] = age;
    stu["Score"].append(score[0]);
    stu["Score"].append(score[1]);
    stu["Score"].append(score[2]);

    std::string str;
    if(Serialize(stu, str) == false)
    {
        return -1;
    }
    std::cout << str << std::endl;
    std::cout << "-------------------------------" << std::endl;

    Json::Value val;
    if(UnSerialize(str, val) == false)
    {
        return -1;
    }

    std::cout << val["Name"].asString() << std::endl;
    std::cout << val["Age"].asInt() << std::endl;
    for (int i = 0; i < val["Score"].size(); i++)
    {
        std::cout << val["Score"][i].asInt() << std::endl;
    }

    return 0;
}