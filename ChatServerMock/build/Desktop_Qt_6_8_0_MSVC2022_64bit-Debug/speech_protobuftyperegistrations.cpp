
#include "speech.qpb.h"

#include <QtProtobuf/qprotobufregistration.h>

namespace SnowK {
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarSpeechRecognitionReq(qRegisterProtobufType<SpeechRecognitionReq>);
static QtProtobuf::ProtoTypeRegistrar ProtoTypeRegistrarSpeechRecognitionRsp(qRegisterProtobufType<SpeechRecognitionRsp>);
static bool RegisterSpeechProtobufTypes = [](){ qRegisterProtobufTypes(); return true; }();
} // namespace SnowK

