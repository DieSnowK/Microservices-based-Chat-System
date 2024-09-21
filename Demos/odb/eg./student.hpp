#pragma once
#include <string>
#include <cstddef>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/nullable.hxx>
#include <odb/core.hxx>

#pragma db object // 声明一个类是数据库对象
class Student
{
public:
    Student() {}
    Student(unsigned long sn, const std::string &name, unsigned short age, unsigned long cid) 
        : _sn(sn)
        , _name(name)
        , _age(age)
        , _classes_id(cid) 
    {}

    void sn(unsigned long num) { _sn = num; }
    unsigned long sn() { return _sn; }

    void name(const std::string &name) { _name = name; }
    std::string name() { return _name; }

    void age(unsigned short num) { _age = num; }
    odb::nullable<unsigned short> age() { return _age; }

    void classes_id(unsigned long cid) { _classes_id = cid; }
    unsigned long classes_id() { return _classes_id; }

private:
    // 将odb::access类作为Person类的友元
    // 这是使数据库支持代码可访问默认构造函数和数据成员所必需的
    friend class odb::access;

    // id: 标记成员变量为主键, auto: 值在插入时自动生成
    #pragma db id auto
    unsigned long _id;

    // unique: 指定变量或一组变量具有唯一键约束
    #pragma db unique
    unsigned long _sn;
    std::string _name;
    odb::nullable<unsigned short> _age;

    // 指定成员变量应该被索引
    #pragma db index
    unsigned long _classes_id;
};

#pragma db object
class Classes
{
public:
    Classes() {}
    Classes(const std::string &name) 
        : _name(name) 
    {}

    void name(const std::string &name) { _name = name; }
    std::string name() { return _name; }

private:
    friend class odb::access;

    #pragma db id auto
    unsigned long _id;
    std::string _name;
};

// 查询所有的学生信息, 并显示班级名称
#pragma db view object(Student)\
                object(Classes = classes : Student::_classes_id == classes::_id)\
                query((?)) // 用于自定义查询函数
struct Classes_Student
{
    // 指定类成员映射到数据库表中的列名
    #pragma db column(Student::_id)
    unsigned long id;
    #pragma db column(Student::_sn)
    unsigned long sn;
    #pragma db column(Student::_name)
    std::string name;
    #pragma db column(Student::_age)
    odb::nullable<unsigned short> age;
    #pragma db column(classes::_name)
    std::string classes_name;
};

// 只查询学生姓名, (?): 外部调用时传入的过滤条件
#pragma db view query("select name from Student + (?)")
struct All_Name
{
    std::string name;
};

// odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time student.hpp