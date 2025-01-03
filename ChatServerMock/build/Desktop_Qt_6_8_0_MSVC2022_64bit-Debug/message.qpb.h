/* This file is autogenerated. DO NOT CHANGE. All changes will be lost */

#ifndef MESSAGE_QPB_H
#define MESSAGE_QPB_H

#include "base.qpb.h"

#include <QtProtobuf/qprotobuflazymessagepointer.h>
#include <QtProtobuf/qprotobufmessage.h>
#include <QtProtobuf/qprotobufobject.h>
#include <QtProtobuf/qprotobufoneof.h>
#include <QtProtobuf/qtprotobuftypes.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qstring.h>

#include <optional>

namespace SnowK {
class GetHistoryMsgReq;
using GetHistoryMsgReqRepeated = QList<GetHistoryMsgReq>;
namespace GetHistoryMsgReq_QtProtobufNested {
enum class QtProtobufFieldEnum;
} // namespace GetHistoryMsgReq_QtProtobufNested

class GetHistoryMsgRsp;
using GetHistoryMsgRspRepeated = QList<GetHistoryMsgRsp>;
namespace GetHistoryMsgRsp_QtProtobufNested {
enum class QtProtobufFieldEnum;
} // namespace GetHistoryMsgRsp_QtProtobufNested

class GetRecentMsgReq;
using GetRecentMsgReqRepeated = QList<GetRecentMsgReq>;
namespace GetRecentMsgReq_QtProtobufNested {
enum class QtProtobufFieldEnum;
} // namespace GetRecentMsgReq_QtProtobufNested

class GetRecentMsgRsp;
using GetRecentMsgRspRepeated = QList<GetRecentMsgRsp>;
namespace GetRecentMsgRsp_QtProtobufNested {
enum class QtProtobufFieldEnum;
} // namespace GetRecentMsgRsp_QtProtobufNested

class MsgSearchReq;
using MsgSearchReqRepeated = QList<MsgSearchReq>;
namespace MsgSearchReq_QtProtobufNested {
enum class QtProtobufFieldEnum;
} // namespace MsgSearchReq_QtProtobufNested

class MsgSearchRsp;
using MsgSearchRspRepeated = QList<MsgSearchRsp>;
namespace MsgSearchRsp_QtProtobufNested {
enum class QtProtobufFieldEnum;
} // namespace MsgSearchRsp_QtProtobufNested


class GetHistoryMsgReq_QtProtobufData;
class GetHistoryMsgReq : public QProtobufMessage
{
    Q_PROTOBUF_OBJECT
    Q_PROPERTY(QString requestId READ requestId WRITE setRequestId SCRIPTABLE true)
    Q_PROPERTY(QString chatSessionId READ chatSessionId WRITE setChatSessionId SCRIPTABLE true)
    Q_PROPERTY(QtProtobuf::int64 startTime READ startTime WRITE setStartTime SCRIPTABLE false)
    Q_PROPERTY(QtProtobuf::int64 overTime READ overTime WRITE setOverTime SCRIPTABLE false)
    Q_PROPERTY(QString userId READ userId_p WRITE setUserId_p)
    Q_PROPERTY(bool hasUserId READ hasUserId)
    Q_PROPERTY(QString sessionId READ sessionId_p WRITE setSessionId_p)
    Q_PROPERTY(bool hasSessionId READ hasSessionId)

public:
    using QtProtobufFieldEnum = GetHistoryMsgReq_QtProtobufNested::QtProtobufFieldEnum;
    GetHistoryMsgReq();
    ~GetHistoryMsgReq();
    GetHistoryMsgReq(const GetHistoryMsgReq &other);
    GetHistoryMsgReq &operator =(const GetHistoryMsgReq &other);
    GetHistoryMsgReq(GetHistoryMsgReq &&other) noexcept;
    GetHistoryMsgReq &operator =(GetHistoryMsgReq &&other) noexcept
    {
        swap(other);
        return *this;
    }
    void swap(GetHistoryMsgReq &other) noexcept
    {
        QProtobufMessage::swap(other);
        dptr.swap(other.dptr);
    }

    const QString &requestId() const &;

    const QString &chatSessionId() const &;

    QtProtobuf::int64 startTime() const;

    QtProtobuf::int64 overTime() const;

    bool hasUserId() const;
    QString userId() const;

    bool hasSessionId() const;
    QString sessionId() const;
    void setRequestId(const QString &requestId);
    void setRequestId(QString &&requestId);
    void setChatSessionId(const QString &chatSessionId);
    void setChatSessionId(QString &&chatSessionId);
    void setStartTime(QtProtobuf::int64 startTime);
    void setOverTime(QtProtobuf::int64 overTime);
    void setUserId(const QString &userId);
    void setUserId(QString &&userId);
    void clearUserId();
    void setSessionId(const QString &sessionId);
    void setSessionId(QString &&sessionId);
    void clearSessionId();
    static void registerTypes();

private:
    friend bool comparesEqual(const GetHistoryMsgReq &lhs, const GetHistoryMsgReq &rhs) noexcept;
    friend bool operator==(const GetHistoryMsgReq &lhs, const GetHistoryMsgReq &rhs) noexcept
    {
        return comparesEqual(lhs, rhs);
    }
    friend bool operator!=(const GetHistoryMsgReq &lhs, const GetHistoryMsgReq &rhs) noexcept
    {
        return !comparesEqual(lhs, rhs);
    }
    QString userId_p() const;
    QString sessionId_p() const;
    void setUserId_p(QString userId);
    void setSessionId_p(QString sessionId);
    QExplicitlySharedDataPointer<GetHistoryMsgReq_QtProtobufData> dptr;
};
namespace GetHistoryMsgReq_QtProtobufNested {
Q_NAMESPACE

enum class QtProtobufFieldEnum {
    RequestIdProtoFieldNumber = 1,
    ChatSessionIdProtoFieldNumber = 2,
    StartTimeProtoFieldNumber = 3,
    OverTimeProtoFieldNumber = 4,
    UserIdProtoFieldNumber = 5,
    SessionIdProtoFieldNumber = 6,
};
Q_ENUM_NS(QtProtobufFieldEnum)

} // namespace GetHistoryMsgReq_QtProtobufNested

class GetHistoryMsgRsp_QtProtobufData;
class GetHistoryMsgRsp : public QProtobufMessage
{
    Q_PROTOBUF_OBJECT
    Q_PROPERTY(QString requestId READ requestId WRITE setRequestId SCRIPTABLE true)
    Q_PROPERTY(bool success READ success WRITE setSuccess SCRIPTABLE true)
    Q_PROPERTY(QString errmsg READ errmsg WRITE setErrmsg SCRIPTABLE true)
    Q_PROPERTY(SnowK::MessageInfoRepeated msgListData READ msgList WRITE setMsgList SCRIPTABLE true)

public:
    using QtProtobufFieldEnum = GetHistoryMsgRsp_QtProtobufNested::QtProtobufFieldEnum;
    GetHistoryMsgRsp();
    ~GetHistoryMsgRsp();
    GetHistoryMsgRsp(const GetHistoryMsgRsp &other);
    GetHistoryMsgRsp &operator =(const GetHistoryMsgRsp &other);
    GetHistoryMsgRsp(GetHistoryMsgRsp &&other) noexcept;
    GetHistoryMsgRsp &operator =(GetHistoryMsgRsp &&other) noexcept
    {
        swap(other);
        return *this;
    }
    void swap(GetHistoryMsgRsp &other) noexcept
    {
        QProtobufMessage::swap(other);
        dptr.swap(other.dptr);
    }

    const QString &requestId() const &;

    bool success() const;

    const QString &errmsg() const &;

    const MessageInfoRepeated &msgList() const &;
    void setRequestId(const QString &requestId);
    void setRequestId(QString &&requestId);
    void setSuccess(bool success);
    void setErrmsg(const QString &errmsg);
    void setErrmsg(QString &&errmsg);
    void setMsgList(const MessageInfoRepeated &msgList);
    void setMsgList(MessageInfoRepeated &&msgList);
    static void registerTypes();

private:
    friend bool comparesEqual(const GetHistoryMsgRsp &lhs, const GetHistoryMsgRsp &rhs) noexcept;
    friend bool operator==(const GetHistoryMsgRsp &lhs, const GetHistoryMsgRsp &rhs) noexcept
    {
        return comparesEqual(lhs, rhs);
    }
    friend bool operator!=(const GetHistoryMsgRsp &lhs, const GetHistoryMsgRsp &rhs) noexcept
    {
        return !comparesEqual(lhs, rhs);
    }
    QExplicitlySharedDataPointer<GetHistoryMsgRsp_QtProtobufData> dptr;
};
namespace GetHistoryMsgRsp_QtProtobufNested {
Q_NAMESPACE

enum class QtProtobufFieldEnum {
    RequestIdProtoFieldNumber = 1,
    SuccessProtoFieldNumber = 2,
    ErrmsgProtoFieldNumber = 3,
    MsgListProtoFieldNumber = 4,
};
Q_ENUM_NS(QtProtobufFieldEnum)

} // namespace GetHistoryMsgRsp_QtProtobufNested

class GetRecentMsgReq_QtProtobufData;
class GetRecentMsgReq : public QProtobufMessage
{
    Q_PROTOBUF_OBJECT
    Q_PROPERTY(QString requestId READ requestId WRITE setRequestId SCRIPTABLE true)
    Q_PROPERTY(QString chatSessionId READ chatSessionId WRITE setChatSessionId SCRIPTABLE true)
    Q_PROPERTY(QtProtobuf::int64 msgCount READ msgCount WRITE setMsgCount SCRIPTABLE false)
    Q_PROPERTY(QtProtobuf::int64 curTime READ curTime_p WRITE setCurTime_p)
    Q_PROPERTY(bool hasCurTime READ hasCurTime)
    Q_PROPERTY(QString userId READ userId_p WRITE setUserId_p)
    Q_PROPERTY(bool hasUserId READ hasUserId)
    Q_PROPERTY(QString sessionId READ sessionId_p WRITE setSessionId_p)
    Q_PROPERTY(bool hasSessionId READ hasSessionId)

public:
    using QtProtobufFieldEnum = GetRecentMsgReq_QtProtobufNested::QtProtobufFieldEnum;
    GetRecentMsgReq();
    ~GetRecentMsgReq();
    GetRecentMsgReq(const GetRecentMsgReq &other);
    GetRecentMsgReq &operator =(const GetRecentMsgReq &other);
    GetRecentMsgReq(GetRecentMsgReq &&other) noexcept;
    GetRecentMsgReq &operator =(GetRecentMsgReq &&other) noexcept
    {
        swap(other);
        return *this;
    }
    void swap(GetRecentMsgReq &other) noexcept
    {
        QProtobufMessage::swap(other);
        dptr.swap(other.dptr);
    }

    const QString &requestId() const &;

    const QString &chatSessionId() const &;

    QtProtobuf::int64 msgCount() const;

    bool hasCurTime() const;
    QtProtobuf::int64 curTime() const;

    bool hasUserId() const;
    QString userId() const;

    bool hasSessionId() const;
    QString sessionId() const;
    void setRequestId(const QString &requestId);
    void setRequestId(QString &&requestId);
    void setChatSessionId(const QString &chatSessionId);
    void setChatSessionId(QString &&chatSessionId);
    void setMsgCount(QtProtobuf::int64 msgCount);
    void setCurTime(QtProtobuf::int64 curTime);
    void clearCurTime();
    void setUserId(const QString &userId);
    void setUserId(QString &&userId);
    void clearUserId();
    void setSessionId(const QString &sessionId);
    void setSessionId(QString &&sessionId);
    void clearSessionId();
    static void registerTypes();

private:
    friend bool comparesEqual(const GetRecentMsgReq &lhs, const GetRecentMsgReq &rhs) noexcept;
    friend bool operator==(const GetRecentMsgReq &lhs, const GetRecentMsgReq &rhs) noexcept
    {
        return comparesEqual(lhs, rhs);
    }
    friend bool operator!=(const GetRecentMsgReq &lhs, const GetRecentMsgReq &rhs) noexcept
    {
        return !comparesEqual(lhs, rhs);
    }
    QtProtobuf::int64 curTime_p() const;
    QString userId_p() const;
    QString sessionId_p() const;
    void setCurTime_p(QtProtobuf::int64 curTime);
    void setUserId_p(QString userId);
    void setSessionId_p(QString sessionId);
    QExplicitlySharedDataPointer<GetRecentMsgReq_QtProtobufData> dptr;
};
namespace GetRecentMsgReq_QtProtobufNested {
Q_NAMESPACE

enum class QtProtobufFieldEnum {
    RequestIdProtoFieldNumber = 1,
    ChatSessionIdProtoFieldNumber = 2,
    MsgCountProtoFieldNumber = 3,
    CurTimeProtoFieldNumber = 4,
    UserIdProtoFieldNumber = 5,
    SessionIdProtoFieldNumber = 6,
};
Q_ENUM_NS(QtProtobufFieldEnum)

} // namespace GetRecentMsgReq_QtProtobufNested

class GetRecentMsgRsp_QtProtobufData;
class GetRecentMsgRsp : public QProtobufMessage
{
    Q_PROTOBUF_OBJECT
    Q_PROPERTY(QString requestId READ requestId WRITE setRequestId SCRIPTABLE true)
    Q_PROPERTY(bool success READ success WRITE setSuccess SCRIPTABLE true)
    Q_PROPERTY(QString errmsg READ errmsg WRITE setErrmsg SCRIPTABLE true)
    Q_PROPERTY(SnowK::MessageInfoRepeated msgListData READ msgList WRITE setMsgList SCRIPTABLE true)

public:
    using QtProtobufFieldEnum = GetRecentMsgRsp_QtProtobufNested::QtProtobufFieldEnum;
    GetRecentMsgRsp();
    ~GetRecentMsgRsp();
    GetRecentMsgRsp(const GetRecentMsgRsp &other);
    GetRecentMsgRsp &operator =(const GetRecentMsgRsp &other);
    GetRecentMsgRsp(GetRecentMsgRsp &&other) noexcept;
    GetRecentMsgRsp &operator =(GetRecentMsgRsp &&other) noexcept
    {
        swap(other);
        return *this;
    }
    void swap(GetRecentMsgRsp &other) noexcept
    {
        QProtobufMessage::swap(other);
        dptr.swap(other.dptr);
    }

    const QString &requestId() const &;

    bool success() const;

    const QString &errmsg() const &;

    const MessageInfoRepeated &msgList() const &;
    void setRequestId(const QString &requestId);
    void setRequestId(QString &&requestId);
    void setSuccess(bool success);
    void setErrmsg(const QString &errmsg);
    void setErrmsg(QString &&errmsg);
    void setMsgList(const MessageInfoRepeated &msgList);
    void setMsgList(MessageInfoRepeated &&msgList);
    static void registerTypes();

private:
    friend bool comparesEqual(const GetRecentMsgRsp &lhs, const GetRecentMsgRsp &rhs) noexcept;
    friend bool operator==(const GetRecentMsgRsp &lhs, const GetRecentMsgRsp &rhs) noexcept
    {
        return comparesEqual(lhs, rhs);
    }
    friend bool operator!=(const GetRecentMsgRsp &lhs, const GetRecentMsgRsp &rhs) noexcept
    {
        return !comparesEqual(lhs, rhs);
    }
    QExplicitlySharedDataPointer<GetRecentMsgRsp_QtProtobufData> dptr;
};
namespace GetRecentMsgRsp_QtProtobufNested {
Q_NAMESPACE

enum class QtProtobufFieldEnum {
    RequestIdProtoFieldNumber = 1,
    SuccessProtoFieldNumber = 2,
    ErrmsgProtoFieldNumber = 3,
    MsgListProtoFieldNumber = 4,
};
Q_ENUM_NS(QtProtobufFieldEnum)

} // namespace GetRecentMsgRsp_QtProtobufNested

class MsgSearchReq_QtProtobufData;
class MsgSearchReq : public QProtobufMessage
{
    Q_PROTOBUF_OBJECT
    Q_PROPERTY(QString requestId READ requestId WRITE setRequestId SCRIPTABLE true)
    Q_PROPERTY(QString userId READ userId_p WRITE setUserId_p)
    Q_PROPERTY(bool hasUserId READ hasUserId)
    Q_PROPERTY(QString sessionId READ sessionId_p WRITE setSessionId_p)
    Q_PROPERTY(bool hasSessionId READ hasSessionId)
    Q_PROPERTY(QString chatSessionId READ chatSessionId WRITE setChatSessionId SCRIPTABLE true)
    Q_PROPERTY(QString searchKey READ searchKey WRITE setSearchKey SCRIPTABLE true)

public:
    using QtProtobufFieldEnum = MsgSearchReq_QtProtobufNested::QtProtobufFieldEnum;
    MsgSearchReq();
    ~MsgSearchReq();
    MsgSearchReq(const MsgSearchReq &other);
    MsgSearchReq &operator =(const MsgSearchReq &other);
    MsgSearchReq(MsgSearchReq &&other) noexcept;
    MsgSearchReq &operator =(MsgSearchReq &&other) noexcept
    {
        swap(other);
        return *this;
    }
    void swap(MsgSearchReq &other) noexcept
    {
        QProtobufMessage::swap(other);
        dptr.swap(other.dptr);
    }

    const QString &requestId() const &;

    bool hasUserId() const;
    QString userId() const;

    bool hasSessionId() const;
    QString sessionId() const;

    const QString &chatSessionId() const &;

    const QString &searchKey() const &;
    void setRequestId(const QString &requestId);
    void setRequestId(QString &&requestId);
    void setUserId(const QString &userId);
    void setUserId(QString &&userId);
    void clearUserId();
    void setSessionId(const QString &sessionId);
    void setSessionId(QString &&sessionId);
    void clearSessionId();
    void setChatSessionId(const QString &chatSessionId);
    void setChatSessionId(QString &&chatSessionId);
    void setSearchKey(const QString &searchKey);
    void setSearchKey(QString &&searchKey);
    static void registerTypes();

private:
    friend bool comparesEqual(const MsgSearchReq &lhs, const MsgSearchReq &rhs) noexcept;
    friend bool operator==(const MsgSearchReq &lhs, const MsgSearchReq &rhs) noexcept
    {
        return comparesEqual(lhs, rhs);
    }
    friend bool operator!=(const MsgSearchReq &lhs, const MsgSearchReq &rhs) noexcept
    {
        return !comparesEqual(lhs, rhs);
    }
    QString userId_p() const;
    QString sessionId_p() const;
    void setUserId_p(QString userId);
    void setSessionId_p(QString sessionId);
    QExplicitlySharedDataPointer<MsgSearchReq_QtProtobufData> dptr;
};
namespace MsgSearchReq_QtProtobufNested {
Q_NAMESPACE

enum class QtProtobufFieldEnum {
    RequestIdProtoFieldNumber = 1,
    UserIdProtoFieldNumber = 2,
    SessionIdProtoFieldNumber = 3,
    ChatSessionIdProtoFieldNumber = 4,
    SearchKeyProtoFieldNumber = 5,
};
Q_ENUM_NS(QtProtobufFieldEnum)

} // namespace MsgSearchReq_QtProtobufNested

class MsgSearchRsp_QtProtobufData;
class MsgSearchRsp : public QProtobufMessage
{
    Q_PROTOBUF_OBJECT
    Q_PROPERTY(QString requestId READ requestId WRITE setRequestId SCRIPTABLE true)
    Q_PROPERTY(bool success READ success WRITE setSuccess SCRIPTABLE true)
    Q_PROPERTY(QString errmsg READ errmsg WRITE setErrmsg SCRIPTABLE true)
    Q_PROPERTY(SnowK::MessageInfoRepeated msgListData READ msgList WRITE setMsgList SCRIPTABLE true)

public:
    using QtProtobufFieldEnum = MsgSearchRsp_QtProtobufNested::QtProtobufFieldEnum;
    MsgSearchRsp();
    ~MsgSearchRsp();
    MsgSearchRsp(const MsgSearchRsp &other);
    MsgSearchRsp &operator =(const MsgSearchRsp &other);
    MsgSearchRsp(MsgSearchRsp &&other) noexcept;
    MsgSearchRsp &operator =(MsgSearchRsp &&other) noexcept
    {
        swap(other);
        return *this;
    }
    void swap(MsgSearchRsp &other) noexcept
    {
        QProtobufMessage::swap(other);
        dptr.swap(other.dptr);
    }

    const QString &requestId() const &;

    bool success() const;

    const QString &errmsg() const &;

    const MessageInfoRepeated &msgList() const &;
    void setRequestId(const QString &requestId);
    void setRequestId(QString &&requestId);
    void setSuccess(bool success);
    void setErrmsg(const QString &errmsg);
    void setErrmsg(QString &&errmsg);
    void setMsgList(const MessageInfoRepeated &msgList);
    void setMsgList(MessageInfoRepeated &&msgList);
    static void registerTypes();

private:
    friend bool comparesEqual(const MsgSearchRsp &lhs, const MsgSearchRsp &rhs) noexcept;
    friend bool operator==(const MsgSearchRsp &lhs, const MsgSearchRsp &rhs) noexcept
    {
        return comparesEqual(lhs, rhs);
    }
    friend bool operator!=(const MsgSearchRsp &lhs, const MsgSearchRsp &rhs) noexcept
    {
        return !comparesEqual(lhs, rhs);
    }
    QExplicitlySharedDataPointer<MsgSearchRsp_QtProtobufData> dptr;
};
namespace MsgSearchRsp_QtProtobufNested {
Q_NAMESPACE

enum class QtProtobufFieldEnum {
    RequestIdProtoFieldNumber = 1,
    SuccessProtoFieldNumber = 2,
    ErrmsgProtoFieldNumber = 3,
    MsgListProtoFieldNumber = 4,
};
Q_ENUM_NS(QtProtobufFieldEnum)

} // namespace MsgSearchRsp_QtProtobufNested
} // namespace SnowK

#endif // MESSAGE_QPB_H
