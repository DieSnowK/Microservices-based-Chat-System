#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <gflags/gflags.h>
#include "student.hpp"
#include "student-odb.hxx"

DEFINE_string(host, "127.0.0.1", "Mysql服务器地址");
DEFINE_int32(port, 0, "Mysql服务器端口");
DEFINE_string(db, "TestDB", "数据库默认库名称");
DEFINE_string(user, "root", "Mysql用户名");
DEFINE_string(pwd, "SnowK8989", "Mysql密码");
DEFINE_string(cset, "utf8", "Mysql客户端字符集");
DEFINE_int32(max_pool, 3, "Mysql连接池最大连接数量");

void Insert_Classes(odb::mysql::database& db)
{
    try
    {
        // 3.获取事务对象, 开启事务
        odb::transaction trans(db.begin());

        Classes c1("Electronic 221");
        Classes c2("Electronic 222");

        db.persist(c1);
        db.persist(c2);

        // 5.提交事务
        trans.commit();
    }
    catch(const std::exception& e)
    {
        std::cout << "插入数据出错: " << e.what() << std::endl;
    }
}

void Insert_Student(odb::mysql::database &db)
{
    try
    {
        // 3.获取事务对象, 开启事务
        odb::transaction trans(db.begin());

        Student s1(1, "张三", 18, 1);
        Student s2(2, "李四", 19, 1);
        Student s3(3, "王五", 18, 1);
        Student s4(4, "赵六", 15, 2);
        Student s5(5, "刘七", 18, 2);
        Student s6(6, "孙八", 23, 2);
        db.persist(s1);
        db.persist(s2);
        db.persist(s3);
        db.persist(s4);
        db.persist(s5);
        db.persist(s6);

        // 5. 提交事务
        trans.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << "插入学生数据出错: " << e.what() << std::endl;
    }
}

// 先查询，再修改
void Update_Student(odb::mysql::database &db, Student &stu)
{
    try
    {
        // 3.获取事务对象, 开启事务
        odb::transaction trans(db.begin());
        db.update(stu);

        // 5. 提交事务
        trans.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << "更新学生数据出错: " << e.what() << std::endl;
    }
}

// TODO
Student Select_Student(odb::mysql::database &db)
{
    Student ret;
    try
    {
        // 3.获取事务对象, 开启事务
        odb::transaction trans(db.begin());

        odb::result<Student> r(db.query<Student>(odb::query<Student>::name == "张三"));
        if (r.size() != 1)
        {
            std::cout << "数据量不对" << std::endl;
            return Student();
        }

        ret = *r.begin();

        // 5. 提交事务
        trans.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << "更新学生数据出错: " << e.what() << std::endl;
    }

    return ret;
}

void Remove_Student(odb::mysql::database &db)
{
    try
    {
        // 3.获取事务对象, 开启事务
        odb::transaction trans(db.begin());

        // 查询和删除操作合并
        db.erase_query<Student>(odb::query<Student>::name == "李四");

        // 5. 提交事务
        trans.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << "更新学生数据出错: " << e.what() << std::endl;
    }
}

void Classes_Student(odb::mysql::database &db)
{
    try
    {
        // 3.获取事务对象, 开启事务
        odb::transaction trans(db.begin());

        typedef odb::query<struct Classes_Student> query;
        typedef odb::result<struct Classes_Student> result;
        
        result r(db.query<struct Classes_Student>(query::classes::id == 1));
        for (auto it = r.begin(); it != r.end(); ++it)
        {
            std::cout << it->id << std::endl;
            std::cout << it->sn << std::endl;
            std::cout << it->name << std::endl;
            std::cout << *it->age << std::endl; // nullable类型类似智能指针, 需要解引用
            std::cout << it->classes_name << std::endl;
        }

        // 5. 提交事务
        trans.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << "更新学生数据出错: " << e.what() << std::endl;
    }
}

void All_Student(odb::mysql::database &db)
{
    try
    {
        // 3.获取事务对象, 开启事务
        odb::transaction trans(db.begin());

        typedef odb::query<Student> query;
        typedef odb::result<All_Name> result;

        result r(db.query<All_Name>(query::id == 1));
        for (auto it = r.begin(); it != r.end(); ++it)
        {
            std::cout << it->name << std::endl;
        }

        // 5. 提交事务
        trans.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << "查询所有学生姓名数据出错: " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    // 1.构造连接池工厂配置对象, 这里只能用unique_ptr
    auto cpf = std::make_unique<odb::mysql::connection_pool_factory>(FLAGS_max_pool, 0);

    // 2.构造数据库操作对象
    odb::mysql::database db(FLAGS_user, FLAGS_pwd, FLAGS_db, FLAGS_host, 
                            FLAGS_port, "", FLAGS_cset, 0, std::move(cpf));

    // 4.数据操作
    // Insert_Classes(db);
    // Insert_Student(db);
    // {
    //     Student stu = Select_Student(db);
    //     stu.age(22);
    //     Update_Student(db, stu);
    // }
    // Remove_Student(db);
    // Classes_Student(db);
    All_Student(db);

    return 0;
}