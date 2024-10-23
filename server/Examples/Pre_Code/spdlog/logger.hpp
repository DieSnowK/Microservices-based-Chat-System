#pragma once
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace SnowK
{
    std::shared_ptr<spdlog::logger> g_logger;

    // true: Release Mode; false: Debug Mode
    void InitLogger(bool mode, const std::string &file, size_t level)
    {
        if(mode == false)
        {
            // Debug mode, create a standard output logger with the lowest output level
            g_logger = spdlog::stdout_color_mt("default-logger");
            g_logger->set_level(spdlog::level::level_enum::trace);
            g_logger->flush_on(spdlog::level::level_enum::trace);
        }
        else
        {
            // Release mode, create a file output log, and the output level is based on the parameters
            g_logger = spdlog::basic_logger_mt("default-logger", file);
            g_logger->set_level((spdlog::level::level_enum)level); // scoped enum
            g_logger->flush_on((spdlog::level::level_enum)level);
        }

        g_logger->set_pattern("[%n][%H:%M:%S][%t][%-8l]%v");
    }

    // __VA_ARGS__: Extract variadics
    // ##: Make sure that no extra commas are generated when there are no variadas to avoid syntax errors
    #define LOG_TRACE(format, ...) SnowK::g_logger->trace(std::string("[{}:{}]") + format, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_DEBUG(format, ...) SnowK::g_logger->debug(std::string("[{}:{}]") + format, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_INFO(format, ...) SnowK::g_logger->info(std::string("[{}:{}]") + format, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_WARN(format, ...) SnowK::g_logger->warn(std::string("[{}:{}]") + format, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_ERROR(format, ...) SnowK::g_logger->error(std::string("[{}:{}]") + format, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_FATAL(format, ...) SnowK::g_logger->critical(std::string("[{}:{}]") + format, __FILE__, __LINE__, ##__VA_ARGS__)
}