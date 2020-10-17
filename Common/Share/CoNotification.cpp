#include "Share/CoNotification.h"

CoNotification::CoNotification(Reactor* pReactor) :
	_pReactor(pReactor)
{
}


CoNotification::~CoNotification()
{
}


void CoNotification::setSocket(const Socket& socket)
{
	_socket = socket;
}


ReadableNotification::ReadableNotification(Reactor* pReactor) :
	CoNotification(pReactor)
{
}


ReadableNotification::~ReadableNotification()
{
}


WritableNotification::WritableNotification(Reactor* pReactor) :
	CoNotification(pReactor)
{
}


WritableNotification::~WritableNotification()
{
}


ErrorNotification::ErrorNotification(Reactor* pReactor) :
	CoNotification(pReactor)
{
}


ErrorNotification::~ErrorNotification()
{
}


TimeoutNotification::TimeoutNotification(Reactor* pReactor) :
	CoNotification(pReactor)
{
}


TimeoutNotification::~TimeoutNotification()
{
}


IdleNotification::IdleNotification(Reactor* pReactor) :
	CoNotification(pReactor)
{
}


IdleNotification::~IdleNotification()
{
}


ShutdownNotification::ShutdownNotification(Reactor* pReactor) :
	CoNotification(pReactor)
{
}


ShutdownNotification::~ShutdownNotification()
{
}