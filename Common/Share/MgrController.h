#pragma once
#include "EntityEventHandler.h"
#include <Hpp/NonCopyable.h>
#include <unordered_map>
#include <set>

class MgrController : public NonCopyable
{
    using MgrMap = std::unordered_map<EntityEventType, std::set<EntityEventHandler*>>;
public:
    void addMgr(EntityEventType event, EntityEventHandler* handler);
    void removeMgr(EntityEventType event, EntityEventHandler* handler);
    void onNotify(EntityEventType event);
    bool empty() const
    {
        return handlersMap_.empty();
    }
    void clear()
    {
        handlersMap_.clear();
    }

private:
    MgrMap handlersMap_;
};
