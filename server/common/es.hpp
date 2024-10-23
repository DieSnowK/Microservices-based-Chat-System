#include "elasticsearch.hpp"

namespace SnowK
{
    class ESClientFactory
    {
    public:
        static std::shared_ptr<elasticlient::Client> Create(const std::vector<std::string> host_list)
        {
            return std::make_shared<elasticlient::Client>(host_list);
        }
    };
}