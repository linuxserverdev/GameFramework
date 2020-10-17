#include "GateEngineImpl.h"
#include "GateEntityMgr.h"
#include <Share/RpcController.h>
GateEngine& GateEngine::getIns()
{
	return GateEngineImpl::getIns();
}

bool GateEngineImpl::loadEngine(GatewayFrame& frame)
{
    GateEntityMgr::getIns().loadGlobalEntity(RpcController::getIns());



    return true;
}
