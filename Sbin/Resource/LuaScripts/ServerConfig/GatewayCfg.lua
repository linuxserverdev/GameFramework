gatewayCfg = {}

gatewayCfg.logger = {
	LogPath     = "../Log/Gateway",
	LogBaseName = "GatewayServer",
	LogExtName  = ".log",
	LogLevel    = "debug", -- log level:[debug|trace|info|warn|error|fatal]
	FileSize    = "20M",
}

-- rpc type:[RPC_CLIENT|RPC_GATEWAY|RPC_SESSION|RPC_WORLD|RPC_DBX]
gatewayCfg.listener = {
	RPC_CLIENT = "127.0.0.1:20001",
}
gatewayCfg.connector = {
	RPC_SESSION = "127.0.0.1:20002",
}


-- loader.loadLogger = function(buff)
--     local luabuf = tolua.cast(buff, "LuaByteBuffer")
-- 	if luabuf then
-- 	    local logger = config.logger
-- 		luabuf:pushString(logger and logger.logPatn or "")
-- 		luabuf:pushString(logger and logger.logBaseName or "")
-- 		luabuf:pushString(logger and logger.logExtName or "")
-- 		luabuf:pushString(logger and logger.LogLevel or "")
-- 		luabuf:pushString(logger and logger.FileSize or "")
-- 		print("load gateway logger config")
-- 	end
-- end
--
-- return loader