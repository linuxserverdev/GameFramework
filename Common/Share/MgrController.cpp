#include "MgrController.h"
#include <Logger/Logger.h>

void MgrController::addMgr(EntityEventType event, EntityEventHandler* handler)
{
    MgrMap::iterator iter = handlersMap_.find(event);
    if (iter != handlersMap_.end())
    {
        std::set<EntityEventHandler*>& handlers = iter->second;
        std::set<EntityEventHandler*>::iterator iterSet = handlers.find(handler);
        if (iterSet == handlers.end())
        {
            handlers.emplace(handler);
            return;
        }
        LOG_ERROR << "MgrEvent already exist";
    }
    handlersMap_.emplace(event, std::set<EntityEventHandler*>{handler});
}

void MgrController::removeMgr(EntityEventType event, EntityEventHandler* handler)
{
    MgrMap::iterator iter = handlersMap_.find(event);
    if (iter != handlersMap_.end())
    {
        std::set<EntityEventHandler*>& handlers = iter->second;
        handlers.erase(handler);
    }
}

void MgrController::onNotify(EntityEventType event)
{
    MgrMap::const_iterator iter = handlersMap_.find(event);
    if (iter != handlersMap_.end())
    {
        const std::set<EntityEventHandler*>& handlers = iter->second;
        for(const auto& mgr : handlers)
        {
            mgr->onNotify(event);
        }
        return;
    }
    LOG_WARN << "MgrEventHandler not exist, MgrEventType = " << event;
}
