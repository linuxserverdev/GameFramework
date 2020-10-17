#include "RpcController.h"
#include <Logger/Logger.h>

void RpcController::pushMsg(MsgPtr msg)
{
	if (!msg)
	{
		LOG_WARN << "push empty msg";
		return;
	}

	if (msg->empty())
	{
		LOG_WARN << "push empty msg";
		return;
	}
	{
		FastScopedLock lock(_mutex);
		_MsgList.push_back(msg);
	}
	_cond.signal();
}


void RpcController::run()
{
	while (!_stop)
	{
		MsgList tmpMsgList;
		{
			FastScopedLock lock(_mutex);
			if (_MsgList.empty() && _cond.tryWait(_mutex, 10000))
			{
				if (!_MsgList.empty())
				{
					tmpMsgList.swap(_MsgList);
				}
			}
			else
				tmpMsgList.swap(_MsgList);
		}
		if (!tmpMsgList.empty())
		{
			for (const MsgPtr& msgList : tmpMsgList)
			{
				for (const RpcMsgPacket& msg : *msgList)
				{
					onRecvProto(msg);
				}
			}
			tmpMsgList.clear();
		}
	}
}

void RpcController::regProtoEventCS(const SysIDType& sysID, RpcEventHandler* handler)
{
	if (handler)
	{
		LOG_INFO << "reg CS SysID:" << sysID;
		_RpcEventCS[sysID] = handler;
	}
}

void RpcController::regProtoEventSS(const SysIDType& sysID, RpcEventHandler* handler)
{
	if (handler)
	{
		LOG_INFO << "reg SS SysID:" << sysID;
		_RpcEventSS[sysID] = handler;
	}
}

void RpcController::onRecvProto(const RpcMsgPacket& msg)
{
	switch (msg.msgHead_.rpcType_)
	{
	case RPC_CLIENT:
		onRecvProtoCS(msg); return;
	default:
		onRecvProtoSS(msg); return;
	}
}

void RpcController::onRecvProtoCS(const RpcMsgPacket& msg)
{
	RpcEventMap::const_iterator iter = _RpcEventCS.find(msg.msgHead_.sysID_);
	if (iter != _RpcEventCS.end())
	{
		if (iter->second)
		{
			iter->second->onRecvProtoCS(msg); return;
		}
	}
	LOG_WARN << "CS RpcEventHandler not exist, (sysID, protoID) = "
		<< "(" << msg.msgHead_.sysID_ << "," << msg.msgHead_.protoID_ << ")";
}

void RpcController::onRecvProtoSS(const RpcMsgPacket& msg)
{
	RpcEventMap::const_iterator iter = _RpcEventSS.find(msg.msgHead_.sysID_);
	if (iter != _RpcEventSS.end())
	{
		if (iter->second)
		{
			iter->second->onRecvProtoSS(msg); return;
		}
	}
	LOG_WARN << "SS RpcEventHandler not exist, (sysID, protoID) = "
		<< "(" << msg.msgHead_.sysID_ << "," << msg.msgHead_.protoID_ << ")";
}