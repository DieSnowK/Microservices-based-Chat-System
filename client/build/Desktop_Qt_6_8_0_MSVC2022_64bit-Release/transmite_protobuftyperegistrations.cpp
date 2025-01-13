
#include "transmite.qpb.h"

#include <QtProtobuf/qprotobufregistration.h>

namespace SnowK {
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarNewMessageReq(qRegisterProtobufType<NewMessageReq>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarNewMessageRsp(qRegisterProtobufType<NewMessageRsp>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarGetTransmitTargetRsp(qRegisterProtobufType<GetTransmitTargetRsp>);
static bool RegisterTransmiteProtobufTypes = [](){ qRegisterProtobufTypes(); return true; }();
} // namespace SnowK

