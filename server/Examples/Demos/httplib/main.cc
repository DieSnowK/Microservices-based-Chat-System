#include "httplib.h"

int main()
{
    // 1.实例化服务器对象
    httplib::Server svr;

    // 2.注册回调函数  void(const httplib::Request&, const httplib::Response&)
    svr.Get("/SnowK", [](const httplib::Request& req, httplib::Response& resp)
    {
        std::cout << req.method << std::endl;
        std::cout << req.path << std::endl;
        for(auto& iter : req.headers)
        {
            std::cout << iter.first << ": " << iter.second << std::endl;
        }

        std::string body("<html><body><h1>Hello SnowK<h1><body><html>");
        resp.set_content(body, "text/html");
        resp.status = 200;
    });

    // 3.启动服务器
    svr.listen("0.0.0.0", 9200);

    return 0;
}
