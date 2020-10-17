#pragma once
#include "RpcMsg.h"
#include "Share/MsgBuffer.h"
class RpcMsgParser
{
public:
    bool parse(MsgBuffer* buffer);
    bool gotAll(MsgPtr& msg);
private:
    MsgPtr packets_;
    bool gotAll_{false};
};