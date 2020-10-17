#include "LoginProto.h"
#include "LoginMgr.h"
#include "LoginCst.h"
#include <Logger/Logger.h>
#include <Share/RpcController.h>


LoginProto& LoginProto::getIns()
{
    static LoginProto instance;
    return instance;
}

void LoginProto::regEvent(RpcController& rpc)
{
    rpc.regProtoEventCS(CSSysID_LOGIN,this);
    rpc.regProtoEventSS(SSSysID_LOGIN, this);
}

void LoginProto::onRecvProtoCS(const RpcMsgPacket& msg)
{

    switch (msg.msgHead_.protoID_)
    {
    case CS_RPC_EVENT_LOGIN:
        //onRecvLogin(LoginMgr& mgr, const std::string& pb)
        break;
    case CS_RPC_EVENT_LOGOUT:
        //onRecvLogout(LoginMgr& mgr, const std::string& pb)
        break;
    default:
        LOG_INFO << "non CS handler message,sysID:" << msg.msgHead_.sysID_
                    <<" protoID:" << msg.msgHead_.protoID_;
        return;
    }
}

void LoginProto::onRecvLoginCS(LoginMgr& mgr, const std::string& pb)
{

}

void LoginProto::onRecvLogoutCS(LoginMgr& mgr, const std::string& pb)
{

}

void LoginProto::onRecvProtoSS(const RpcMsgPacket& msg)
{
	switch (msg.msgHead_.protoID_)
	{
	case SS_RPC_EVENT_LOGIN:
		
		break;
	case SS_RPC_EVENT_LOGOUT:
		
		break;
	default:
		LOG_INFO << "non SS handler message,sysID:" << msg.msgHead_.sysID_
			<< " protoID:" << msg.msgHead_.protoID_;
		return;
	}
}

