#include "GateServer.h"
#include <GateInterface/GateEngine.h>
#include <Share/Utils.h>
#include <Share/FileSystem.h>
#include <Logger/LogStream.h>
#include <Logger/AsyncFileLogger.h>
#include <Logger/Logger.h>

LogStream& operator<<(LogStream &los, std::map<std::string, std::string> cfgMap)
{
    for (auto& cfg : cfgMap)
    {
        los << cfg.first << ":" << cfg.second << " ";
    }
    return los;
}

GatewayFrame& GatewayFrame::getIns()
{
    return GatewayServer::getIns();
}

void GatewayServer::runGateway(std::map<std::string, std::map<std::string, std::string>>& gatewayCfg)
{
    //setting async file logger config
    if(utils::createPath(gatewayCfg["logger"]["LogPath"]) != 0)
    {
        LOG_ERROR << "create logger path failed";
        return;
    }
    AsyncFileLogger& asyncFileLogger = AsyncFileLogger::getIns();
    asyncFileLogger.setFileName(gatewayCfg["logger"]["LogBaseName"],
            gatewayCfg["logger"]["LogExtName"],
            gatewayCfg["logger"]["LogPath"]);
    utils::setServerLogLevel(gatewayCfg["logger"]["LogLevel"]);
    size_t fileSize{0};
    if(utils::bytesSize(gatewayCfg["logger"]["FileSize"], fileSize))
    {
        if(fileSize > 0)
        {
            asyncFileLogger.setFileSizeLimit(fileSize);
            LOG_DEBUG << "fileSize = " << fileSize;
        }
    }
    //start async file logger
    asyncFileLogger.startLogging();
    Logger::setOutputFunction(
            [](const char *msg, const uint64_t len) {
				AsyncFileLogger::getIns().output(msg, len);
#ifndef NDEBUG
                fwrite(msg, 1, len, stdout);
#endif
            },
            []{
				AsyncFileLogger::getIns().flush();
#ifndef NDEBUG
                fflush(stdout);
#endif
            });
    LOG_INFO << "start async file logger - " << gatewayCfg["logger"];

    if (!GateEngine::getIns().loadEngine(*this))
    {
        LOG_ERROR << "load gate engine failed";
        return;
    }

    //start listenners
    for (const auto& item : gatewayCfg["listener"])
    {
        const std::string& name  = item.first;
        const std::string& value = item.second;
        auto ipPort = utils::splitString(value, ":");
        RpcType type = RPC_INVALID;
        if((type = RpcMsgPacket::getRpcType(name)) == RPC_INVALID)
        {
            LOG_ERROR << "err listener:" + name;
            return;
        }
        SocketAddress sa(ipPort[0], std::stoi(ipPort[1]));
        ServerSocket ss(sa);
		switch (type)
		{
		case RPC_CLIENT:
            {
                _listeners.push_back(
					new Listener<RpcServiceHandler>(ss, _reactor, RPC_CLIENT));
            }            
			break;
		case RPC_GATEWAY:
		    {
                _listeners.push_back(
					new Listener<RpcServiceHandler>(ss, _reactor, RPC_GATEWAY));
		    }			
			break;
		case RPC_SESSION:
		    {
                _listeners.push_back(
					new Listener<RpcServiceHandler>(ss, _reactor, RPC_SESSION));
		    }
			break;
		case RPC_WORLD:
		    {
                _listeners.push_back(
					new Listener<RpcServiceHandler>(ss, _reactor, RPC_WORLD));
		    }
			break;
		default:
            LOG_ERROR << "invalid rpc type:" << name; 
            return;
		}
    }
    //start connectors
	for (const auto& item : gatewayCfg["connector"])
	{
		const std::string& name = item.first;
		const std::string& value = item.second;
		auto ipPort = utils::splitString(value, ":");
		RpcType type = RPC_INVALID;
		if ((type = RpcMsgPacket::getRpcType(name)) == RPC_INVALID)
		{
			LOG_ERROR << "err listener:" + name;
			return;
		}
		SocketAddress sa(ipPort[0], std::stoi(ipPort[1]));
		switch (type)
		{
		case RPC_CLIENT:
			{
				_connectors.push_back(
					new Connector<RpcServiceHandler>(sa, _reactor, RPC_CLIENT));
			}
			break;
		case RPC_GATEWAY:
			{
				_connectors.push_back(
					new Connector<RpcServiceHandler>(sa, _reactor, RPC_GATEWAY));
			}
			break;
		case RPC_SESSION:
			{
				_connectors.push_back(
					new Connector<RpcServiceHandler>(sa, _reactor, RPC_SESSION));
			}
			break;
		case RPC_WORLD:
			{	
				_connectors.push_back(
					new Connector<RpcServiceHandler>(sa, _reactor, RPC_WORLD));
			}
			break;
		default:
			LOG_ERROR << "invalid rpc type:" << name;
			return;
		}
	}

	_thread.start(RpcController::getIns());
	_reactor.run();
	_thread.join();
}


GatewayServer::~GatewayServer()
{
	for (auto p : _listeners)
	{
        delete p;
	}
	for (auto p : _connectors)
	{
		delete p;
	}
}
