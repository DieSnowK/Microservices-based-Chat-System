#pragma once
#include "aip-cpp-sdk/speech.h"
#include "logger.hpp"

namespace SnowK
{
    class ASRClient
    {
    public:
        using ptr = std::shared_ptr<ASRClient>;

        ASRClient(const std::string &app_id, const std::string &api_key, const std::string &secret_key)
            : _client(app_id, api_key, secret_key)
        {}

        std::string Recognize(const std::string &speech_data, std::string &err)
        {
            Json::Value result = _client.recognize(speech_data, "pcm", 16000, aip::null);

            if (result["err_no"].asInt() != 0)
            {
                err = result["err_msg"].asString();
                LOG_ERROR("Faild to recognize: {}", err);

                return std::string();
            }

            return result["result"][0].asString();
        }

    private: 
        aip::Speech _client;
    };
}