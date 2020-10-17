#pragma once
#include <Hpp/NonCopyable.h>

class GatewayFrame;
class GateEngine : public NonCopyable
{
public:
	static GateEngine& getIns();
	virtual bool loadEngine(GatewayFrame& frame) = 0;
};
