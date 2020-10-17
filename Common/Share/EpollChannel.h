#pragma once
#include "Poco/RefCountedObject.h"

enum ChannelType
{
	SocketChannel,
	EventChannel
};

class EpollChannel : public Poco::RefCountedObject
{
public:
	EpollChannel(ChannelType type, void* ptr):_type(type), _ptr(ptr){}
	ChannelType _type;
	void* _ptr;
};
