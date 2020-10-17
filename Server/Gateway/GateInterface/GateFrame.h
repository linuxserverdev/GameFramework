#pragma once
#include <Hpp/NonCopyable.h>

class GatewayFrame : public NonCopyable
{
public:
    static GatewayFrame& getIns();
    virtual ~GatewayFrame() {}
};