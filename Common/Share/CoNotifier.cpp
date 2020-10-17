#include "Share/CoNotifier.h"
#include "Share/CoNotification.h"
#include "Share/Reactor.h"

CoNotifier::CoNotifier(const Socket& socket) :
	_socket(socket)
{
}


CoNotifier::~CoNotifier()
{
}


void CoNotifier::addObserver(Reactor* pReactor, const Poco::AbstractObserver& observer)
{
	_nc.addObserver(observer);
	ScopedLock l(_mutex);
	if (observer.accepts(pReactor->_pReadableNotification))
		_events.insert(pReactor->_pReadableNotification.get());
	else if (observer.accepts(pReactor->_pWritableNotification))
		_events.insert(pReactor->_pWritableNotification.get());
	else if (observer.accepts(pReactor->_pErrorNotification))
		_events.insert(pReactor->_pErrorNotification.get());
	else if (observer.accepts(pReactor->_pTimeoutNotification))
		_events.insert(pReactor->_pTimeoutNotification.get());
}


void CoNotifier::removeObserver(Reactor* pReactor, const Poco::AbstractObserver& observer)
{
	_nc.removeObserver(observer);
	ScopedLock l(_mutex);
	EventSet::iterator it = _events.end();
	if (observer.accepts(pReactor->_pReadableNotification))
		it = _events.find(pReactor->_pReadableNotification.get());
	else if (observer.accepts(pReactor->_pWritableNotification))
		it = _events.find(pReactor->_pWritableNotification.get());
	else if (observer.accepts(pReactor->_pErrorNotification))
		it = _events.find(pReactor->_pErrorNotification.get());
	else if (observer.accepts(pReactor->_pTimeoutNotification))
		it = _events.find(pReactor->_pTimeoutNotification.get());
	if (it != _events.end())
		_events.erase(it);
}


namespace
{
	static Socket nullSocket;
}


void CoNotifier::dispatch(CoNotification* pNotification)
{
	pNotification->setSocket(_socket);
	pNotification->duplicate();
	try
	{
		_nc.postNotification(pNotification);
	}
	catch (...)
	{
		pNotification->setSocket(nullSocket);
		throw;
	}
	pNotification->setSocket(nullSocket);
}