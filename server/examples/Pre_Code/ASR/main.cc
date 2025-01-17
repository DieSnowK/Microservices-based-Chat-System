#include "asr.hpp"

int main()
{
    std::string app_id = "";
    std::string api_key = "";
    std::string secret_key = "";

    SnowK::ASRClient client(app_id, api_key, secret_key);

    std::string file_content, err;
    aip::get_file_content("16k.pcm", &file_content);

    std::string ret = client.Recognize(file_content, err);
    if(ret.empty())
    {
        std::cout << "语音识别出错" << std::endl;
        return -1;
    }

    std::cout << ret << std::endl;

    return 0;
}