#pragma once
#include <string>
#include <cstddef>
#include <boost/date_time/posix_time/posix_time.hpp>

// ��C++�У�Ҫʹ��ODB��������Ϊ�־û��࣬��Ҫ����ODB�ĺ���ͷ�ļ�����ʹ��#pragma db objectָ��
// #pragma db object ָʾ ODB �������� person ����Ϊһ���־û���
#include <odb/core.hxx>

typedef boost::posix_time::ptime ptime;

#pragma db object
class Person
{
public:
    Person(const std::string &name, int age, const ptime &update)
        : _name(name), _age(age), _update(update)
    {}

    void age(int val) { _age = val; }
    int age() { return _age; }

    void name(const std::string &val) { _name = val; }
    std::string name() { return _name; }

    void update(const ptime &update) { _update = update; }
    std::string update() { return boost::posix_time::to_simple_string(_update); }

private:
    // �� odb����access ����Ϊ person ������ѡ�
    // ����ʹ���ݿ�֧�ִ���ɷ���Ĭ�Ϲ��캯�������ݳ�Ա������ġ�
    // �������й���Ĭ�Ϲ��캯���͹������ݳ�Ա�����ݳ�Ա�Ĺ��������������η�������Ҫ��Ԫ����
    friend class odb::access;
    Person() {}

// _id ��Աǰ��� pragma ���� ODB �����������³�Ա�Ƕ���ı�ʶ��
// auto˵����ָʾ�������ݿ����� ID
#pragma db id auto      // ��ʾ ID �ֶν��Զ����ɣ�ͨ�������ݿ��е��������� 
    unsigned long _id;
    unsigned short _age;
    std::string _name;
#pragma db type("TIMESTAMP") not_null
    boost::posix_time::ptime _update;
};
// �� ODB ����ָʾ�����һ�𣬲������ඨ��֮������Ҳ�����ƶ���һ�������ı�ͷ�У�ʹԭʼ����ȫ���ֲ���
// #pragma db object(person)
// #pragma db member(person::_name) id
// ��ɺ���Ҫʹ�� odb ����������ǰ��д�Ĵ����������ݿ�֧�ִ���
// odb -d mysql --generate-query --generate-schema person.hxx
// ����õ��� boost ���еĽӿڣ�����Ҫʹ��ѡ�� : --profile boost/datetime
// odb -d mysql --generate-query --generate-schema --profile boost/date-time person.hxx