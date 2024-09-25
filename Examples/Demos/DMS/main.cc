#include <cstdlib>
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/CommonRequest.h>
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/CommonResponse.h>

using namespace std;
using namespace AlibabaCloud;

int main(int argc, char **argv)
{
    AlibabaCloud::InitializeSdk();
    AlibabaCloud::ClientConfiguration configuration("cn-hangzhou");
    // specify timeout when create client.
    configuration.setConnectTimeout(1500);
    configuration.setReadTimeout(4000);
    // Please ensure that the environment variables ALIBABA_CLOUD_ACCESS_KEY_ID and ALIBABA_CLOUD_ACCESS_KEY_SECRET are set.
    AlibabaCloud::Credentials credential(getenv("ALIBABA_CLOUD_ACCESS_KEY_ID"), getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET"));
    /* use STS Token
    credential.setSessionToken( getenv("ALIBABA_CLOUD_SECURITY_TOKEN") );
    */
    AlibabaCloud::CommonClient client(credential, configuration);
    AlibabaCloud::CommonRequest request(AlibabaCloud::CommonRequest::RequestPattern::RpcPattern);
    request.setHttpMethod(AlibabaCloud::HttpRequest::Method::Post);
    request.setDomain("dysmsapi.aliyuncs.com");
    request.setVersion("2017-05-25");
    request.setQueryParameter("Action", "SendSms");
    request.setQueryParameter("SignName", "SnowK6");
    request.setQueryParameter("TemplateCode", "SMS_474020138");
    request.setQueryParameter("PhoneNumbers", "18351958129");
    request.setQueryParameter("TemplateParam", "{\"code\":\"1234\"}");

    auto response = client.commonResponse(request);
    if (response.isSuccess())
    {
        printf("request success.\n");
        printf("result: %s\n", response.result().payload().c_str());
    }
    else
    {
        printf("error: %s\n", response.error().errorMessage().c_str());
        printf("request id: %s\n", response.error().requestId().c_str());
    }

    AlibabaCloud::ShutdownSdk();
    return 0;
}