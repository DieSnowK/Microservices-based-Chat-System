
#include "message.qpb.h"

#include <QtProtobuf/qprotobufregistration.h>

namespace SnowK {
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarGetHistoryMsgReq(qRegisterProtobufType<GetHistoryMsgReq>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarGetHistoryMsgRsp(qRegisterProtobufType<GetHistoryMsgRsp>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarGetRecentMsgReq(qRegisterProtobufType<GetRecentMsgReq>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarGetRecentMsgRsp(qRegisterProtobufType<GetRecentMsgRsp>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarMsgSearchReq(qRegisterProtobufType<MsgSearchReq>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarMsgSearchRsp(qRegisterProtobufType<MsgSearchRsp>);
static bool RegisterMessageProtobufTypes = [](){ qRegisterProtobufTypes(); return true; }();
} // namespace SnowK

