#include "GateServer.h"
#include <Share/FileSystem.h>
#include <LuaEngine/LuaEngine.h>
#ifndef _WIN32
#include <signal.h>
#include <unistd.h>
#include <libgen.h>
#endif


int main()
{
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    std::map<std::string, std::map<std::string, std::string>> gatewayCfg;
    FileSystem::getIns().addFindPath("../Resource/LuaScripts/");
    {
        LuaEngine lua;
        if (!lua.create())
        {
            LOG_ERROR << "create lua engine failed:" << lua.getLastError();
            return -1;;
        }
        std::string configFile("ServerConfig/GatewayCfg.lua");
        if (!lua.loadLuaFile(configFile)) 
        {
            LOG_ERROR << "load lua file " << configFile << " failed:" << lua.getLastError();
            return -1;
        }
        if (!lua.getGlobalVariable("gatewayCfg", gatewayCfg)) 
        {
            LOG_ERROR << "getGlobalVariable gatewayCfg failed:" << lua.getLastError();
            return -1;
        }
    }

    GatewayServer::getIns().runGateway(gatewayCfg);
}





//        for (auto &cfgItem : gatewayCfg) {
//            LOG_INFO << "cfg:" << cfgItem.first;
//            for (auto &logCfgItem : cfgItem.second) {
//                LOG_INFO << "cfg item:" << logCfgItem.first;
//                LOG_INFO << "cfg value:" << logCfgItem.second;
//            }
//        }

//ByteBuffer& operator>>(ByteBuffer &buffer, LoggerInfo& logInfo)
//{
//    buffer >> logInfo.logPath >> logInfo.logBaseName
//           >> logInfo.logExtName >> logInfo.logLevel >> logInfo.fileSize;
//    return buffer;
//}

//LogStream& operator<<(LogStream &los, LoggerInfo& logInfo)
//{
//    los << "logPath=" << logInfo.logPath;
//    los << " logBaseName=" << logInfo.logBaseName;
//    los << " logExtName=" << logInfo.logExtName;
//    los << " LogLevel=" << logInfo.logLevel;
//    los << " LogFileSize=" << logInfo.fileSize;
//    return los;
//}


//        {
//            //getGlobalVariable test
//            std::string loglevel("server_config/logLevel.lua");
//            if(!lua.loadLuaFile(loglevel))
//            {
//                LOG_ERROR << "load lua file " << loglevel << " failed:" << lua.getLastError();
//                return -1;;
//            }
//            std::map<std::string, std::map<std::string, std::string>> cfg;
//            if (!lua.getGlobalVariable("dirsvr", cfg))
//            {
//                LOG_ERROR << "getGlobalVariable dirsvr failed:" << lua.getLastError();
//                return -1;
//            }
//
//            std::map<std::string, std::map<std::string, std::string> >::const_iterator
//                    it = cfg.begin();
//            for(; it != cfg.end(); ++it)
//            {
//                const std::string &filter = it->first;
//                LOG_INFO << "filter:" << filter;
//                std::map<std::string, std::string>::const_iterator
//                        _it = it->second.begin();
//                for (; _it != it->second.end(); ++_it)
//                {
//                    const std::string &level_name = _it->first;
//                    LOG_INFO << "level_name:" << level_name;
//                }
//            }
//        }
//        lua.openLuaByteBuffer();
//        LuaByteBuffer luaBuff;
//        LuaFunctor<Type_Null, UserData> loadLogger(lua.getLuaState(), "gateway.loadLogger");
//        if(!loadLogger(LUA_NOREF, nil, UserData(&luaBuff)))
//        {
//            LOG_ERROR << "run lua function loadGatewayConfig() failed:" << lua.getLastError();
//            return -1;;
//        }
//        ByteBuffer& result = luaBuff.getByteBuffer();
//        LoggerInfo logInfo;
//        result >> logInfo;
//        LOG_DEBUG << logInfo;

