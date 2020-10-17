#pragma once
#include <share/RpcEventHandler.h>

class RpcController;
class LoginMgr;
class LoginProto final : public RpcEventHandler
{
public:
    static LoginProto& getIns();
    void regEvent(RpcController& rpc);
    void onRecvProtoCS(const RpcMsgPacket& msg) override;
    void onRecvProtoSS(const RpcMsgPacket& msg) override;

private:
    void onRecvLoginCS(LoginMgr& mgr, const std::string& pb);
    void onRecvLogoutCS(LoginMgr& mgr, const std::string& pb);
};