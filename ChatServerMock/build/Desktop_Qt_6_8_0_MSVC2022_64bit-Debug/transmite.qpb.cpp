/* This file is autogenerated. DO NOT CHANGE. All changes will be lost */


#include "transmite.qpb.h"

#include <QtProtobuf/qprotobufregistration.h>

#include <cmath>

namespace SnowK {

class NewMessageReq_QtProtobufData : public QSharedData
{
public:
    NewMessageReq_QtProtobufData()
        : QSharedData(),
          m_message(nullptr)
    {
    }

    NewMessageReq_QtProtobufData(const NewMessageReq_QtProtobufData &other)
        : QSharedData(other),
          m_requestId(other.m_requestId),
          m_userId(other.m_userId),
          m_sessionId(other.m_sessionId),
          m_chatSessionId(other.m_chatSessionId),
          m_message(other.m_message
                                               ? new MessageContent(*other.m_message)
                                               : nullptr)
    {
    }

    QString m_requestId;
    std::optional<QString> m_userId;
    std::optional<QString> m_sessionId;
    QString m_chatSessionId;
    QtProtobufPrivate::QProtobufLazyMessagePointer<MessageContent> m_message;
};

NewMessageReq::~NewMessageReq() = default;

static constexpr struct {
    QtProtobufPrivate::QProtobufPropertyOrdering::Data data;
    const std::array<uint, 21> qt_protobuf_NewMessageReq_uint_data;
    const char qt_protobuf_NewMessageReq_char_data[70];
} qt_protobuf_NewMessageReq_metadata {
    // data
    {
        0, /* = version */
        5, /* = num fields */
        6, /* = field number offset */
        11, /* = property index offset */
        16, /* = field flags offset */
        19, /* = message full name length */
    },
    // uint_data
    {
        // JSON name offsets:
        20, /* = requestId */
        30, /* = userId */
        37, /* = sessionId */
        47, /* = chatSessionId */
        61, /* = message */
        69, /* = end-of-string-marker */
        // Field numbers:
        1, /* = requestId */
        2, /* = userId */
        3, /* = sessionId */
        4, /* = chatSessionId */
        5, /* = message */
        // Property indices:
        0, /* = requestId */
        1, /* = userId */
        3, /* = sessionId */
        5, /* = chatSessionId */
        6, /* = message */
        // Field flags:
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = requestId */
        uint(QtProtobufPrivate::FieldFlag::Optional | QtProtobufPrivate::FieldFlag::ExplicitPresence), /* = userId */
        uint(QtProtobufPrivate::FieldFlag::Optional | QtProtobufPrivate::FieldFlag::ExplicitPresence), /* = sessionId */
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = chatSessionId */
        uint(QtProtobufPrivate::FieldFlag::ExplicitPresence | QtProtobufPrivate::FieldFlag::Message), /* = message */
    },
    // char_data
    /* metadata char_data: */
    "SnowK.NewMessageReq\0" /* = full message name */
    /* field char_data: */
    "requestId\0userId\0sessionId\0chatSessionId\0message\0"
};

const QtProtobufPrivate::QProtobufPropertyOrdering NewMessageReq::staticPropertyOrdering = {
    &qt_protobuf_NewMessageReq_metadata.data
};

void NewMessageReq::registerTypes()
{
    qRegisterMetaType<NewMessageReq>();
    qRegisterMetaType<NewMessageReqRepeated>();
}

NewMessageReq::NewMessageReq()
    : QProtobufMessage(&NewMessageReq::staticMetaObject, &NewMessageReq::staticPropertyOrdering),
      dptr(new NewMessageReq_QtProtobufData)
{
}

NewMessageReq::NewMessageReq(const NewMessageReq &other)
    = default;
NewMessageReq &NewMessageReq::operator =(const NewMessageReq &other)
{
    NewMessageReq temp(other);
    swap(temp);
    return *this;
}
NewMessageReq::NewMessageReq(NewMessageReq &&other) noexcept
    = default;
bool comparesEqual(const NewMessageReq &lhs, const NewMessageReq &rhs) noexcept
{
    return operator ==(static_cast<const QProtobufMessage&>(lhs),
                       static_cast<const QProtobufMessage&>(rhs))
        && lhs.dptr->m_requestId == rhs.dptr->m_requestId
        && lhs.dptr->m_userId == rhs.dptr->m_userId
        && lhs.dptr->m_sessionId == rhs.dptr->m_sessionId
        && lhs.dptr->m_chatSessionId == rhs.dptr->m_chatSessionId
        && (lhs.dptr->m_message == rhs.dptr->m_message
            || *lhs.dptr->m_message == *rhs.dptr->m_message);
}

const QString &NewMessageReq::requestId() const &
{
    return dptr->m_requestId;
}

QString NewMessageReq::userId_p() const
{
    return dptr->m_userId ?
        dptr->m_userId.value() : QString();
}

bool NewMessageReq::hasUserId() const
{
    return dptr->m_userId.has_value();
}
QString NewMessageReq::userId() const
{
    Q_ASSERT(dptr->m_userId.has_value());
    return dptr->m_userId.value();
}

QString NewMessageReq::sessionId_p() const
{
    return dptr->m_sessionId ?
        dptr->m_sessionId.value() : QString();
}

bool NewMessageReq::hasSessionId() const
{
    return dptr->m_sessionId.has_value();
}
QString NewMessageReq::sessionId() const
{
    Q_ASSERT(dptr->m_sessionId.has_value());
    return dptr->m_sessionId.value();
}

const QString &NewMessageReq::chatSessionId() const &
{
    return dptr->m_chatSessionId;
}

MessageContent *NewMessageReq::message_p()
{
    if (!dptr->m_message)
        dptr.detach();
    return dptr->m_message.get();
}

bool NewMessageReq::hasMessage() const
{
    return dptr->m_message.operator bool();
}

const MessageContent &NewMessageReq::message() const &
{
    return *dptr->m_message;
}

void NewMessageReq::clearMessage()
{
    if (dptr->m_message) {
        dptr.detach();
        dptr->m_message.reset();
    }
}

void NewMessageReq::setRequestId(const QString &requestId)
{
    if (dptr->m_requestId != requestId) {
        dptr.detach();
        dptr->m_requestId = requestId;
    }
}

void NewMessageReq::setRequestId(QString &&requestId)
{
    if (dptr->m_requestId != requestId) {
        dptr.detach();
        dptr->m_requestId = std::move(requestId);
    }
}

void NewMessageReq::setUserId(const QString &userId)
{
    if (!dptr->m_userId || dptr->m_userId.value() != userId) {
        dptr.detach();
        dptr->m_userId = userId;
    }
}

void NewMessageReq::setUserId(QString &&userId)
{
    if (!dptr->m_userId || dptr->m_userId.value() != userId) {
        dptr.detach();
        dptr->m_userId = std::move(userId);
    }
}

void NewMessageReq::setUserId_p(QString userId)
{
    if (!dptr->m_userId || dptr->m_userId != userId) {
        dptr.detach();
        dptr->m_userId = userId;
    }
}

void NewMessageReq::clearUserId()
{
    if (dptr->m_userId.has_value()) {
        dptr.detach();
        dptr->m_userId.reset();
    }
}
void NewMessageReq::setSessionId(const QString &sessionId)
{
    if (!dptr->m_sessionId || dptr->m_sessionId.value() != sessionId) {
        dptr.detach();
        dptr->m_sessionId = sessionId;
    }
}

void NewMessageReq::setSessionId(QString &&sessionId)
{
    if (!dptr->m_sessionId || dptr->m_sessionId.value() != sessionId) {
        dptr.detach();
        dptr->m_sessionId = std::move(sessionId);
    }
}

void NewMessageReq::setSessionId_p(QString sessionId)
{
    if (!dptr->m_sessionId || dptr->m_sessionId != sessionId) {
        dptr.detach();
        dptr->m_sessionId = sessionId;
    }
}

void NewMessageReq::clearSessionId()
{
    if (dptr->m_sessionId.has_value()) {
        dptr.detach();
        dptr->m_sessionId.reset();
    }
}
void NewMessageReq::setChatSessionId(const QString &chatSessionId)
{
    if (dptr->m_chatSessionId != chatSessionId) {
        dptr.detach();
        dptr->m_chatSessionId = chatSessionId;
    }
}

void NewMessageReq::setChatSessionId(QString &&chatSessionId)
{
    if (dptr->m_chatSessionId != chatSessionId) {
        dptr.detach();
        dptr->m_chatSessionId = std::move(chatSessionId);
    }
}

void NewMessageReq::setMessage_p(MessageContent *message)
{
    if (dptr->m_message.get() != message) {
        dptr.detach();
        dptr->m_message.reset(message);
    }
}

void NewMessageReq::setMessage(const MessageContent &message)
{
    if (*dptr->m_message != message) {
        dptr.detach();
        *dptr->m_message = message;
    }
}

void NewMessageReq::setMessage(MessageContent &&message)
{
    if (*dptr->m_message != message) {
        dptr.detach();
        *dptr->m_message = std::move(message);
    }
}


class NewMessageRsp_QtProtobufData : public QSharedData
{
public:
    NewMessageRsp_QtProtobufData()
        : QSharedData(),
          m_success(false)
    {
    }

    NewMessageRsp_QtProtobufData(const NewMessageRsp_QtProtobufData &other)
        : QSharedData(other),
          m_requestId(other.m_requestId),
          m_success(other.m_success),
          m_errmsg(other.m_errmsg)
    {
    }

    QString m_requestId;
    bool m_success;
    QString m_errmsg;
};

NewMessageRsp::~NewMessageRsp() = default;

static constexpr struct {
    QtProtobufPrivate::QProtobufPropertyOrdering::Data data;
    const std::array<uint, 13> qt_protobuf_NewMessageRsp_uint_data;
    const char qt_protobuf_NewMessageRsp_char_data[46];
} qt_protobuf_NewMessageRsp_metadata {
    // data
    {
        0, /* = version */
        3, /* = num fields */
        4, /* = field number offset */
        7, /* = property index offset */
        10, /* = field flags offset */
        19, /* = message full name length */
    },
    // uint_data
    {
        // JSON name offsets:
        20, /* = requestId */
        30, /* = success */
        38, /* = errmsg */
        45, /* = end-of-string-marker */
        // Field numbers:
        1, /* = requestId */
        2, /* = success */
        3, /* = errmsg */
        // Property indices:
        0, /* = requestId */
        1, /* = success */
        2, /* = errmsg */
        // Field flags:
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = requestId */
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = success */
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = errmsg */
    },
    // char_data
    /* metadata char_data: */
    "SnowK.NewMessageRsp\0" /* = full message name */
    /* field char_data: */
    "requestId\0success\0errmsg\0"
};

const QtProtobufPrivate::QProtobufPropertyOrdering NewMessageRsp::staticPropertyOrdering = {
    &qt_protobuf_NewMessageRsp_metadata.data
};

void NewMessageRsp::registerTypes()
{
    qRegisterMetaType<NewMessageRsp>();
    qRegisterMetaType<NewMessageRspRepeated>();
}

NewMessageRsp::NewMessageRsp()
    : QProtobufMessage(&NewMessageRsp::staticMetaObject, &NewMessageRsp::staticPropertyOrdering),
      dptr(new NewMessageRsp_QtProtobufData)
{
}

NewMessageRsp::NewMessageRsp(const NewMessageRsp &other)
    = default;
NewMessageRsp &NewMessageRsp::operator =(const NewMessageRsp &other)
{
    NewMessageRsp temp(other);
    swap(temp);
    return *this;
}
NewMessageRsp::NewMessageRsp(NewMessageRsp &&other) noexcept
    = default;
bool comparesEqual(const NewMessageRsp &lhs, const NewMessageRsp &rhs) noexcept
{
    return operator ==(static_cast<const QProtobufMessage&>(lhs),
                       static_cast<const QProtobufMessage&>(rhs))
        && lhs.dptr->m_requestId == rhs.dptr->m_requestId
        && lhs.dptr->m_success == rhs.dptr->m_success
        && lhs.dptr->m_errmsg == rhs.dptr->m_errmsg;
}

const QString &NewMessageRsp::requestId() const &
{
    return dptr->m_requestId;
}

bool NewMessageRsp::success() const
{
    return dptr->m_success;
}

const QString &NewMessageRsp::errmsg() const &
{
    return dptr->m_errmsg;
}

void NewMessageRsp::setRequestId(const QString &requestId)
{
    if (dptr->m_requestId != requestId) {
        dptr.detach();
        dptr->m_requestId = requestId;
    }
}

void NewMessageRsp::setRequestId(QString &&requestId)
{
    if (dptr->m_requestId != requestId) {
        dptr.detach();
        dptr->m_requestId = std::move(requestId);
    }
}

void NewMessageRsp::setSuccess(bool success)
{
    if (dptr->m_success != success) {
        dptr.detach();
        dptr->m_success = success;
    }
}

void NewMessageRsp::setErrmsg(const QString &errmsg)
{
    if (dptr->m_errmsg != errmsg) {
        dptr.detach();
        dptr->m_errmsg = errmsg;
    }
}

void NewMessageRsp::setErrmsg(QString &&errmsg)
{
    if (dptr->m_errmsg != errmsg) {
        dptr.detach();
        dptr->m_errmsg = std::move(errmsg);
    }
}


class GetTransmitTargetRsp_QtProtobufData : public QSharedData
{
public:
    GetTransmitTargetRsp_QtProtobufData()
        : QSharedData(),
          m_success(false),
          m_message(nullptr)
    {
    }

    GetTransmitTargetRsp_QtProtobufData(const GetTransmitTargetRsp_QtProtobufData &other)
        : QSharedData(other),
          m_requestId(other.m_requestId),
          m_success(other.m_success),
          m_errmsg(other.m_errmsg),
          m_message(other.m_message
                                               ? new MessageInfo(*other.m_message)
                                               : nullptr),
          m_targetIdList(other.m_targetIdList)
    {
    }

    QString m_requestId;
    bool m_success;
    QString m_errmsg;
    QtProtobufPrivate::QProtobufLazyMessagePointer<MessageInfo> m_message;
    QStringList m_targetIdList;
};

GetTransmitTargetRsp::~GetTransmitTargetRsp() = default;

static constexpr struct {
    QtProtobufPrivate::QProtobufPropertyOrdering::Data data;
    const std::array<uint, 21> qt_protobuf_GetTransmitTargetRsp_uint_data;
    const char qt_protobuf_GetTransmitTargetRsp_char_data[74];
} qt_protobuf_GetTransmitTargetRsp_metadata {
    // data
    {
        0, /* = version */
        5, /* = num fields */
        6, /* = field number offset */
        11, /* = property index offset */
        16, /* = field flags offset */
        26, /* = message full name length */
    },
    // uint_data
    {
        // JSON name offsets:
        27, /* = requestId */
        37, /* = success */
        45, /* = errmsg */
        52, /* = message */
        60, /* = targetIdList */
        73, /* = end-of-string-marker */
        // Field numbers:
        1, /* = requestId */
        2, /* = success */
        3, /* = errmsg */
        4, /* = message */
        5, /* = targetIdList */
        // Property indices:
        0, /* = requestId */
        1, /* = success */
        2, /* = errmsg */
        3, /* = message */
        5, /* = targetIdList */
        // Field flags:
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = requestId */
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = success */
        uint(QtProtobufPrivate::FieldFlag::NoFlags), /* = errmsg */
        uint(QtProtobufPrivate::FieldFlag::ExplicitPresence | QtProtobufPrivate::FieldFlag::Message), /* = message */
        uint(QtProtobufPrivate::FieldFlag::Repeated), /* = targetIdList */
    },
    // char_data
    /* metadata char_data: */
    "SnowK.GetTransmitTargetRsp\0" /* = full message name */
    /* field char_data: */
    "requestId\0success\0errmsg\0message\0targetIdList\0"
};

const QtProtobufPrivate::QProtobufPropertyOrdering GetTransmitTargetRsp::staticPropertyOrdering = {
    &qt_protobuf_GetTransmitTargetRsp_metadata.data
};

void GetTransmitTargetRsp::registerTypes()
{
    qRegisterMetaType<GetTransmitTargetRsp>();
    qRegisterMetaType<GetTransmitTargetRspRepeated>();
}

GetTransmitTargetRsp::GetTransmitTargetRsp()
    : QProtobufMessage(&GetTransmitTargetRsp::staticMetaObject, &GetTransmitTargetRsp::staticPropertyOrdering),
      dptr(new GetTransmitTargetRsp_QtProtobufData)
{
}

GetTransmitTargetRsp::GetTransmitTargetRsp(const GetTransmitTargetRsp &other)
    = default;
GetTransmitTargetRsp &GetTransmitTargetRsp::operator =(const GetTransmitTargetRsp &other)
{
    GetTransmitTargetRsp temp(other);
    swap(temp);
    return *this;
}
GetTransmitTargetRsp::GetTransmitTargetRsp(GetTransmitTargetRsp &&other) noexcept
    = default;
bool comparesEqual(const GetTransmitTargetRsp &lhs, const GetTransmitTargetRsp &rhs) noexcept
{
    return operator ==(static_cast<const QProtobufMessage&>(lhs),
                       static_cast<const QProtobufMessage&>(rhs))
        && lhs.dptr->m_requestId == rhs.dptr->m_requestId
        && lhs.dptr->m_success == rhs.dptr->m_success
        && lhs.dptr->m_errmsg == rhs.dptr->m_errmsg
        && (lhs.dptr->m_message == rhs.dptr->m_message
            || *lhs.dptr->m_message == *rhs.dptr->m_message)
        && lhs.dptr->m_targetIdList == rhs.dptr->m_targetIdList;
}

const QString &GetTransmitTargetRsp::requestId() const &
{
    return dptr->m_requestId;
}

bool GetTransmitTargetRsp::success() const
{
    return dptr->m_success;
}

const QString &GetTransmitTargetRsp::errmsg() const &
{
    return dptr->m_errmsg;
}

MessageInfo *GetTransmitTargetRsp::message_p()
{
    if (!dptr->m_message)
        dptr.detach();
    return dptr->m_message.get();
}

bool GetTransmitTargetRsp::hasMessage() const
{
    return dptr->m_message.operator bool();
}

const MessageInfo &GetTransmitTargetRsp::message() const &
{
    return *dptr->m_message;
}

void GetTransmitTargetRsp::clearMessage()
{
    if (dptr->m_message) {
        dptr.detach();
        dptr->m_message.reset();
    }
}

const QStringList &GetTransmitTargetRsp::targetIdList() const &
{
    return dptr->m_targetIdList;
}

void GetTransmitTargetRsp::setRequestId(const QString &requestId)
{
    if (dptr->m_requestId != requestId) {
        dptr.detach();
        dptr->m_requestId = requestId;
    }
}

void GetTransmitTargetRsp::setRequestId(QString &&requestId)
{
    if (dptr->m_requestId != requestId) {
        dptr.detach();
        dptr->m_requestId = std::move(requestId);
    }
}

void GetTransmitTargetRsp::setSuccess(bool success)
{
    if (dptr->m_success != success) {
        dptr.detach();
        dptr->m_success = success;
    }
}

void GetTransmitTargetRsp::setErrmsg(const QString &errmsg)
{
    if (dptr->m_errmsg != errmsg) {
        dptr.detach();
        dptr->m_errmsg = errmsg;
    }
}

void GetTransmitTargetRsp::setErrmsg(QString &&errmsg)
{
    if (dptr->m_errmsg != errmsg) {
        dptr.detach();
        dptr->m_errmsg = std::move(errmsg);
    }
}

void GetTransmitTargetRsp::setMessage_p(MessageInfo *message)
{
    if (dptr->m_message.get() != message) {
        dptr.detach();
        dptr->m_message.reset(message);
    }
}

void GetTransmitTargetRsp::setMessage(const MessageInfo &message)
{
    if (*dptr->m_message != message) {
        dptr.detach();
        *dptr->m_message = message;
    }
}

void GetTransmitTargetRsp::setMessage(MessageInfo &&message)
{
    if (*dptr->m_message != message) {
        dptr.detach();
        *dptr->m_message = std::move(message);
    }
}

void GetTransmitTargetRsp::setTargetIdList(const QStringList &targetIdList)
{
    if (dptr->m_targetIdList != targetIdList) {
        dptr.detach();
        dptr->m_targetIdList = targetIdList;
    }
}

void GetTransmitTargetRsp::setTargetIdList(QStringList &&targetIdList)
{
    if (dptr->m_targetIdList != targetIdList) {
        dptr.detach();
        dptr->m_targetIdList = std::move(targetIdList);
    }
}

} // namespace SnowK

#include "moc_transmite.qpb.cpp"