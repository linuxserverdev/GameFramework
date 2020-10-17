#pragma once 
#include "RpcEventHandler.h"
#include "Hpp/NonCopyable.h"
#include "Poco/Condition.h"
#include "Poco/Runnable.h"
#include "Poco/Mutex.h"
#include <unordered_map>

using Poco::FastMutex;
using Poco::Condition;
using Poco::Runnable;
using FastScopedLock = Poco::FastMutex::ScopedLock;

using RpcEventMap = std::unordered_map<SysIDType, RpcEventHandler*>;

class RpcController : public Runnable, public NonCopyable
{
public:
	inline static RpcController& getIns()
	{
		static RpcController instance;
		return instance;
	}
	void pushMsg(MsgPtr msg);
	virtual void run() override;
	void stop()
	{
		_stop = true;
	}
	void regProtoEventCS(const SysIDType& sysID, RpcEventHandler* handler);
	void regProtoEventSS(const SysIDType& sysID, RpcEventHandler* handler);
private:
	void onRecvProto(const RpcMsgPacket& msg);
	void onRecvProtoCS(const RpcMsgPacket& msg);
	void onRecvProtoSS(const RpcMsgPacket& msg);
	RpcController() : _stop(false)
	{
	}
private:
	bool		 _stop;
	Condition    _cond;
	FastMutex    _mutex;
	MsgList      _MsgList;
	RpcEventMap  _RpcEventCS;
	RpcEventMap  _RpcEventSS;
};