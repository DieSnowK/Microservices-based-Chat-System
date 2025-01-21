#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <atomic>
#include <random>
#include <iomanip>
#include "logger.hpp"

namespace SnowK
{
    namespace Utils
    {
        // 生成一个由16位随机字符组成的字符串作为唯一ID
        std::string UUID()
        {
            // 1.生成6个0~255之间的随机数字 -- 生成12位16进制字符
            std::random_device rd;                                   // 实例化设备随机数对象, 用于生成设备随机数
            std::mt19937 generator(rd());                            // 以设备随机数为种子, 实例化伪随机数对象
            std::uniform_int_distribution<int> distribution(0, 255); // 限定数据范围

            std::stringstream ss;
            for (int i = 0; i < 6; i++)
            {
                ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generator);
            }

            // 2.通过一个静态变量生成一个2字节的编号数字 -> 生成4位16进制数字字符
            static std::atomic<short> idx(0);
            short tmp = idx.fetch_add(1);
            ss << std::setw(4) << std::setfill('0') << std::hex << tmp;

            return ss.str();
        }

        std::string VerifyCode()
        {
            std::random_device rd;                                 // 实例化设备随机数对象-用于生成设备随机数
            std::mt19937 generator(rd());                          // 以设备随机数为种子，实例化伪随机数对象
            std::uniform_int_distribution<int> distribution(0, 9); // 限定数据范围

            std::stringstream ss;
            for (int i = 0; i < 4; i++)
            {
                ss << distribution(generator);
            }
            return ss.str();
        }

        bool ReadFile(const std::string &filename, std::string &body)
        {
            std::ifstream ifs(filename, std::ios::binary | std::ios::in);
            if (ifs.is_open() == false)
            {
                LOG_ERROR("Failed to open file {}", filename);
                return false;
            }

            ifs.seekg(0, std::ios::end); // 跳转到文件末尾
            size_t flen = ifs.tellg();   // 获取当前偏移量 -> 文件大小
            ifs.seekg(0, std::ios::beg); // 跳转到文件起始

            body.resize(flen);
            ifs.read(&body[0], flen);
            if (ifs.good() == false)
            {
                LOG_ERROR("Failed to read file", filename);
                ifs.close();
                return false;
            }

            ifs.close();
            return true;
        }

        bool WriteFile(const std::string &filename, const std::string &body)
        {
            std::ofstream ofs(filename, std::ios::out | std::ios::binary | std::ios::trunc);
            if (ofs.is_open() == false)
            {
                LOG_ERROR("Failed to open file {}", filename);
                return false;
            }

            ofs.write(body.c_str(), body.size());
            if (ofs.good() == false)
            {
                LOG_ERROR("Failed to write file", filename);
                ofs.close();
                return false;
            }

            ofs.close();
            return true;
        }
    }
}