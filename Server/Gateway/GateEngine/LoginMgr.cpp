#include "LoginMgr.h"
#include "GateEngine/GateEntity.h"
#include "GateEngine/GateEvent.h"

LoginMgr::LoginMgr(GateEntity* entity)
	: entity_(entity)
{
    entity_->addMgr(GateEntityEvent::kEntityLoad, this);
}

void LoginMgr::onNotify(EntityEventType event)
{

}
