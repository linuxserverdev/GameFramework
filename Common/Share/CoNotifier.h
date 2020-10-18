#pragma once
#include "Share/CoNotificationCenter.h"

#include "Poco/Net/Net.h"
#include "Poco/Net/Socket.h"
#include "Poco/RefCountedObject.h"
#include "Poco/Observer.h"
#include <set>

using Poco::Net::Socket;

class Reactor;
class CoNotification;


class CoNotifier : public Poco::RefCountedObject
	/// This class is used internally by Reactor
	/// to notify registered event handlers of socket events.
{
public:
	explicit CoNotifier(const Socket& socket);
	/// Creates the CoNotifier for the given socket.

	void addObserver(Reactor* pReactor, const Poco::AbstractObserver& observer);
	/// Adds the given observer. 

	void removeObserver(Reactor* pReactor, const Poco::AbstractObserver& observer);
	/// Removes the given observer. 

	bool hasObserver(const Poco::AbstractObserver& observer) const;
	/// Returns true if the given observer is registered.

	bool accepts(CoNotification* pNotification);
	/// Returns true if there is at least one observer for the given notification.

	void dispatch(CoNotification* pNotification);
	/// Dispatches the notification to all observers.

	bool hasObservers() const;
	/// Returns true if there are subscribers.

	std::size_t countObservers() const;
	/// Returns the number of subscribers;

protected:
	~CoNotifier();
	/// Destroys the CoNotifier.

private:
	typedef std::multiset<CoNotification*>     EventSet;

	EventSet                 _events;
	CoNotificationCenter     _nc;
	Socket                   _socket;
};


//
// inlines
//
inline bool CoNotifier::accepts(CoNotification* pNotification)
{
	return _events.find(pNotification) != _events.end();
}


inline bool CoNotifier::hasObserver(const Poco::AbstractObserver& observer) const
{
	return _nc.hasObserver(observer);
}


inline bool CoNotifier::hasObservers() const
{
	return _nc.hasObservers();
}


inline std::size_t CoNotifier::countObservers() const
{
	return _nc.countObservers();
}
