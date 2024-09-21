#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include "person.hpp"
#include "person-odb.hxx"

int main()
{
    std::shared_ptr<odb::core::database> db(new odb::mysql::database
            ("root", "SnowK8989", "TestDB", "127.0.0.1", 0, 0, "utf8"));
    if (!db)
    {
        return -1;
    }

    ptime p = boost::posix_time::second_clock::local_time();
    Person Die("Die", 18, p);
    Person SnowK("SnowK", 19, p);

    typedef odb::query<Person> query;
    typedef odb::result<Person> result;

    // 新增数据
    {
        odb::core::transaction t(db->begin());
        size_t zid = db->persist(Die);
        size_t wid = db->persist(SnowK);
        t.commit();
    }

    // 查询数据
    {
        odb::core::transaction t (db->begin()); 
        result r (db->query<Person>()); 
        for (result::iterator i(r.begin()); i != r.end(); ++i) 
        {
            std::cout << "Hello, " << i->name() << " ";
            std::cout << i->age() << " " << i->update() << std::endl; 
        } 
        t.commit();
    }

    return 0;
}

// 如果用到了boost库中的接口，需要链接库： -lodb-boost
// c++ -o mysql_test mysql_test.cpp person-odb.cxx -lodb-mysqllodb - lodb - boost