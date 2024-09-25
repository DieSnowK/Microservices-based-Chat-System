#include <iostream>
#include <elasticlient/client.h>
#include <cpr/cpr.h>

int main()
{
    // 1.构造ES客户端
    elasticlient::Client client({"http://127.0.0.1:9200/"});

    // 2.发起搜索请求
    try
    {
        auto resp = client.search("user", "_doc", "{\"query\": { \"match_all\":{} }}");
        std::cout << resp.status_code << std::endl;
        std::cout << resp.text << std::endl;
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}
