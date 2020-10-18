#include "CoNotificationCenter.h"
#include "Poco/Notification.h"
#include "Poco/AutoPtr.h"
#include "Poco/SingletonHolder.h"

CoNotificationCenter::CoNotificationCenter()
{
}

CoNotificationCenter::~CoNotificationCenter()
{
}

void CoNotificationCenter::addObserver(const Poco::AbstractObserver& observer)
{
	_observers.push_back(observer.clone());
}

void CoNotificationCenter::removeObserver(const Poco::AbstractObserver& observer)
{
	for (ObserverList::iterator it = _observers.begin(); it != _observers.end(); ++it)
	{
		if (observer.equals(**it))
		{
			(*it)->disable();
			_observers.erase(it);
			return;
		}
	}
}

bool CoNotificationCenter::hasObserver(const Poco::AbstractObserver& observer) const
{
	for (const auto& p : _observers)
		if (observer.equals(*p)) return true;

	return false;
}

void CoNotificationCenter::postNotification(Poco::Notification::Ptr pNotification)
{
	poco_check_ptr(pNotification);
	ObserverList observersToNotify(_observers);
	for (auto& p : observersToNotify)
	{
		p->notify(pNotification);
	}
}

bool CoNotificationCenter::hasObservers() const
{
	return !_observers.empty();
}

std::size_t CoNotificationCenter::countObservers() const
{
	return _observers.size();
}

namespace
{
	static Poco::SingletonHolder<CoNotificationCenter> sh;
}

CoNotificationCenter& CoNotificationCenter::defaultCenter()
{
	return *sh.get();
}