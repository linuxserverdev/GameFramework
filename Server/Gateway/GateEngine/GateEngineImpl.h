#pragma once
#include <GateInterface/GateEngine.h>

class GateEngineImpl : public GateEngine
{
public:
	inline static GateEngineImpl& getIns()
	{
		static GateEngineImpl instance;
		return instance;
	}
	virtual bool loadEngine(GatewayFrame& frame) override;
};