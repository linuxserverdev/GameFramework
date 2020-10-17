#include "GateEntity.h"
#include "LoginMgr.h"
void GateEntity::load()
{
    loginMgr_ = std::make_unique<LoginMgr>(this);
}
