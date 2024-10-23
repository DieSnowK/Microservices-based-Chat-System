#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main()
{   
    // 1.设置全局的刷新策略
    // (1) 每秒刷新
    spdlog::flush_every(std::chrono::seconds(1));
    // (2) 遇到debug以上等级的日志立即刷新
    spdlog::flush_on(spdlog::level::level_enum::debug);
    // (3) 设置全局的日志输出等级 -> 无所谓, 每个日治其可以独立进行设置
    spdlog::set_level(spdlog::level::level_enum::debug);

    // 2.创建同步日志器(标准输出/文件) --> 工厂接口默认创建的是同步日志器
    auto logger = spdlog::stdout_color_mt("default-logger");

    // 设置日期的刷新策略, 以及设置日志器的输出等级
    // logger->flush_on(spdlog::level::level_enum::debug);
    // logger->set_level(spdlog::level::level_enum::debug);

    // 3.设置日志输出格式
    logger->set_pattern("[%H:%M:%S][%t][%-8l] %v");

    // 4.进行简单的日志输出
    logger->trace("Hello {}", "SnowK");
    logger->debug("Hello {}", "SnowK");
    logger->info("Hello {}", "SnowK");
    logger->warn("Hello {}", "SnowK");
    logger->error("Hello {}", "SnowK");
    logger->critical("Hello {}", "SnowK");

    std::cout << "spdlog 日志输出完毕" << std::endl;

    return 0;
}