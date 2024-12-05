
#include "gateway.qpb.h"

#include <QtProtobuf/qprotobufregistration.h>

namespace SnowK {
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarClientAuthenticationReq(qRegisterProtobufType<ClientAuthenticationReq>);
static bool RegisterGatewayProtobufTypes = [](){ qRegisterProtobufTypes(); return true; }();
} // namespace SnowK

