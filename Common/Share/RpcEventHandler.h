#pragma once
#include "RpcMsg.h"
//#include <Hpp/Hash.h>

//struct RpcEventKey
//{
//    RpcEventKey(SysIDType sysID,
//                ProtoIDType protoID)
//            : sysID_(sysID),
//              protoID_(protoID)
//    {
//    }
//    SysIDType sysID_;
//    ProtoIDType protoID_;
//    bool operator== (const RpcEventKey& rhs) const
//    {
//        return sysID_ == rhs.sysID_ &&
//               protoID_ == rhs.protoID_;
//    }
//};

//namespace std
//{
//    template <>
//    struct hash<RpcEventKey>
//    {
//    public:
//        size_t operator()(RpcEventKey const& rhs) const
//        {
//            size_t hashVal = 0;
//            hashCombine(hashVal, rhs.sysID_);
//            hashCombine(hashVal, rhs.protoID_);
//            return hashVal;
//        }
//    };
//}

class RpcEventHandler
{
public:
    virtual ~RpcEventHandler(){}
    virtual void onRecvProtoCS(const RpcMsgPacket& msg) {};
    virtual void onRecvProtoSS(const RpcMsgPacket& msg) {};
};
