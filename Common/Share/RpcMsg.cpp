#include "RpcMsg.h"
#include <map>

struct RpcTypeMap
{
    RpcTypeMap()
    {
        push(RPC_CLIENT, "RPC_CLIENT");
        push(RPC_GATEWAY, "RPC_GATEWAY");
        push(RPC_SESSION, "RPC_SESSION");
        push(RPC_WORLD, "RPC_WORLD");
        push(RPC_DBX, "RPC_DBX");
    }
    void push(RpcType t, const std::string& s)
    {
        t2s_[t] = s;
        s2t_[s] = t;
    }
    RpcType getType(const std::string& s)
    {
        return s2t_[s];
    }
    std::string getString(RpcType t)
    {
        return t2s_[t];
    }

private:
    std::map<RpcType, std::string> t2s_;
    std::map<std::string, RpcType> s2t_;
};

static RpcTypeMap rMap = RpcTypeMap{};
RpcLenType RpcMsgPacket::maxRpcMsgSize_ = 65535;

std::string RpcMsgPacket::getRpcTypeString(RpcType val)
{
    return rMap.getString(val);
}
RpcType RpcMsgPacket::getRpcType(const std::string &s)
{
    return rMap.getType(s);
}