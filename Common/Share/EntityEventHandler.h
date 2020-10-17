#pragma once
#include <cstdint>
using EntityEventType = uint32_t;
class EntityEventHandler
{
public:
    virtual ~EntityEventHandler(){}
    virtual void onNotify(EntityEventType event) = 0;
};