#pragma once
#include "GateInterface/GateFrame.h"
#include "Share/RpcMsgParser.h"
#include "Share/RpcServiceHandler.h"

#include "Poco/Thread.h"

#include <map>

class GatewayServer final : public GatewayFrame

{
public:
    inline static GatewayServer& getIns()
    {
        static GatewayServer instance;
        return instance;
    }
    ~GatewayServer();
    void runGateway(std::map<std::string, std::map<std::string, std::string>>& gatewayCfg);
private:
    std::vector<Listener<RpcServiceHandler>*> _listeners;
    std::vector<Connector<RpcServiceHandler>*> _connectors;
    Reactor _reactor;
    Poco::Thread  _thread;
};