#include "GateEntityMgr.h"
#include <Share/RpcController.h>
#include "LoginProto.h"

GateEntityMgr& GateEntityMgr::getIns()
{
    static GateEntityMgr instance;
    return instance;
}

void GateEntityMgr::loadGlobalEntity(RpcController& rpc)
{
    LoginProto::getIns().regEvent(rpc);
}



