#pragma once
#include <Hpp/NonCopyable.h>
#include <string>
#include <memory>
#include <vector>

using RpcLenType  = uint16_t;
using SysIDType   = uint16_t;
using ProtoIDType = uint8_t;
using CrcType     = uint8_t;

enum RpcType : uint8_t
{
    RPC_INVALID = 0,
    RPC_CLIENT,
    RPC_GATEWAY,
    RPC_SESSION,
    RPC_WORLD,
    RPC_DBX,

    RPC_ENUM_LIMITS
};

#pragma pack(push, 1)
struct RpcMsgHead
{
    RpcLenType  rpcLen_;
    RpcType     rpcType_;
    SysIDType   sysID_;
    ProtoIDType protoID_;
    CrcType     crc_;
};
#pragma pack(pop)

class RpcMsgPacket : NonCopyable
{
public:
    RpcMsgPacket(const RpcMsgHead& head, 
        std::shared_ptr<std::string> body)
    {
        msgHead_ = head;
        msgBody_ = body;
    }
    static std::string makePacketStream(RpcType rpcType, 
        SysIDType sysID, ProtoIDType protoID, const std::string &content)
    {
        RpcMsgHead h;
        h.rpcLen_  = content.size() + sizeof(RpcMsgHead);
        h.rpcType_ = rpcType;
        h.sysID_   = sysID;
        h.protoID_ = protoID;
        h.crc_     = 0;
        std::string msg;
        msg.append((char*)&h, sizeof(RpcMsgHead));
        msg.append(content);
        return msg;
    }
    /// Set custom data on the connection
    void setContext(const std::shared_ptr<void> &context)
    {
        contextPtr_ = context;
    }
    void setContext(std::shared_ptr<void> &&context)
    {
        contextPtr_ = std::move(context);
    }

    /// Get custom data from the connection
    template <typename T>
    std::shared_ptr<T> getContext() const
    {
        return std::static_pointer_cast<T>(contextPtr_);
    }

    /// Return true if the context is set by user.
    bool hasContext() const
    {
        return (bool)contextPtr_;
    }

    /// Clear the context.
    void clearContext()
    {
        contextPtr_.reset();
    }
    static RpcLenType maxRpcMsgSize_;
    static std::string getRpcTypeString(RpcType val);
    static RpcType getRpcType(const std::string& s);
    RpcMsgHead msgHead_;
    std::shared_ptr<std::string> msgBody_;
    std::shared_ptr<void> contextPtr_;
};

using MsgPtr    = std::shared_ptr<std::vector<RpcMsgPacket>>;
using MsgList   = std::vector<MsgPtr>;

using PacketPtr = std::shared_ptr<RpcMsgPacket>;
using MsgQueue  = std::vector<PacketPtr>;