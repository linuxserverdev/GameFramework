#pragma once

#include <Share/CSSysIDGroup.h>
#include <Share/SSSysIDGroup.h>

enum LoginProtocolSetCS : ProtoIDType
{
	CS_RPC_EVENT_LOGIN,
	CS_RPC_EVENT_LOGOUT,
};

enum LoginProtocolSetSS : ProtoIDType
{
	SS_RPC_EVENT_LOGIN,
	SS_RPC_EVENT_LOGOUT,
};