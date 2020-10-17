#pragma once
#include "Hpp/NonCopyable.h"
class GateEntityMgr : public NonCopyable
{
public:
    static GateEntityMgr& getIns();
    void loadGlobalEntity(class RpcController& rpc);

private:

};