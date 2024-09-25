#include "dms.hpp"

int main(int argc, char *argv[])
{
    SnowK::DMSClient client("", "");
    client.Send("18351958129", "5678");

    return 0;
}