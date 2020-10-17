#include "Share/Reactor.h"
#include "Logger/Logger.h"

#include "Poco/ErrorHandler.h"
#include "Poco/Exception.h"

using Poco::Exception;
using Poco::ErrorHandler;

Reactor::Reactor() :
	_stop(false),
	_timeout(DEFAULT_TIMEOUT),
	_pReadableNotification(new ReadableNotification(this)),
	_pWritableNotification(new WritableNotification(this)),
	_pErrorNotification(new ErrorNotification(this)),
	_pTimeoutNotification(new TimeoutNotification(this)),
	_pIdleNotification(new IdleNotification(this)),
	_pShutdownNotification(new ShutdownNotification(this)),
	_timerQueue(new TimerQueue(this)),
	_threadId(Thread::currentTid())
{
}

Reactor::Reactor(const Poco::Timespan& timeout) :
	_stop(false),
	_timeout(timeout),
	_pReadableNotification(new ReadableNotification(this)),
	_pWritableNotification(new WritableNotification(this)),
	_pErrorNotification(new ErrorNotification(this)),
	_pTimeoutNotification(new TimeoutNotification(this)),
	_pIdleNotification(new IdleNotification(this)),
	_pShutdownNotification(new ShutdownNotification(this)),
	_timerQueue(new TimerQueue(this)),
	_threadId(Thread::currentTid())
{
}

Reactor::~Reactor()
{
}

void Reactor::run()
{
	while (!_stop)
	{
		try
		{
			Poco::Timespan timeout = _timerQueue->getTimeout();
			PollSet::SocketModeMap sm = _pollSet.poll(timeout);
			if (sm.size() > 0)
			{
				PollSet::SocketModeMap::iterator it = sm.begin();
				PollSet::SocketModeMap::iterator end = sm.end();
				for (; it != end; ++it)
				{
					if (it->second & PollSet::POLL_READ)
					{
						dispatch(it->first, _pReadableNotification);
					}
					if (it->second & PollSet::POLL_WRITE) dispatch(it->first, _pWritableNotification);
					if (it->second & PollSet::POLL_ERROR) dispatch(it->first, _pErrorNotification);
				}
			}			
			_timerQueue->processTimers();
			doRunInLoopFuncs();
		}
		catch (Exception& exc)
		{
			ErrorHandler::handle(exc);
		}
		catch (std::exception& exc)
		{
			ErrorHandler::handle(exc);
		}
		catch (...)
		{
			ErrorHandler::handle();
		}
	}
	onShutdown();
}

bool Reactor::hasSocketHandlers()
{
	if (!_pollSet.empty())
	{
		for (auto& p : _handlers)
		{
			if (p.second->accepts(_pReadableNotification) ||
				p.second->accepts(_pWritableNotification) ||
				p.second->accepts(_pErrorNotification)) return true;
		}
	}

	return false;
}

void Reactor::stop()
{
	_stop = true;
}

void Reactor::addEventHandler(const Socket& socket, const Poco::AbstractObserver& observer)
{
	NotifierPtr pNotifier = getNotifier(socket, true);

	if (!pNotifier->hasObserver(observer)) pNotifier->addObserver(this, observer);

	int mode = 0;
	if (pNotifier->accepts(_pReadableNotification)) mode |= PollSet::POLL_READ;
	if (pNotifier->accepts(_pWritableNotification)) mode |= PollSet::POLL_WRITE;
	if (pNotifier->accepts(_pErrorNotification))    mode |= PollSet::POLL_ERROR;
	if (mode) _pollSet.add(socket, mode);
}

bool Reactor::hasEventHandler(const Socket& socket, const Poco::AbstractObserver& observer)
{
	NotifierPtr pNotifier = getNotifier(socket);
	if (!pNotifier) return false;
	if (pNotifier->hasObserver(observer)) return true;
	return false;
}

Reactor::NotifierPtr Reactor::getNotifier(const Socket& socket, bool makeNew)
{
	EventHandlerMap::iterator it = _handlers.find(socket);
	if (it != _handlers.end()) return it->second;
	else if (makeNew) return (_handlers[socket] = new CoNotifier(socket));

	return 0;
}

void Reactor::removeEventHandler(const Socket& socket, const Poco::AbstractObserver& observer)
{
	NotifierPtr pNotifier = getNotifier(socket);
	if (pNotifier && pNotifier->hasObserver(observer))
	{
		if (pNotifier->countObservers() == 1)
		{
			_handlers.erase(socket);
			_pollSet.remove(socket);
		}
		pNotifier->removeObserver(this, observer);
	}
}

void Reactor::abortNotInLoopThread()
{
	LOG_FATAL << "It is forbidden to run loop on threads other than event-loop "
		"thread";
	exit(1);
}

void Reactor::assertInLoopThread()
{
	if (!isInLoopThread())
	{
		abortNotInLoopThread();
	}
};

void Reactor::runInLoop(const Func& cb)
{
	if (isInLoopThread())
	{
		cb();
	}
	else
	{
		_funcs.enqueue(cb);
		_pollSet.postEvent();
	}
}

void Reactor::runInLoop(Func&& cb)
{
	if (isInLoopThread())
	{
		cb();
	}
	else
	{
		_funcs.enqueue(std::move(cb));
		_pollSet.postEvent();
	}
}

TimerId Reactor::runAt(const Date& time, const Func& cb)
{
	auto microSeconds =
		time.microSecondsSinceEpoch() - Date::now().microSecondsSinceEpoch();
	std::chrono::steady_clock::time_point tp =
		std::chrono::steady_clock::now() +
		std::chrono::microseconds(microSeconds);
	return _timerQueue->addTimer(cb, tp, std::chrono::microseconds(0));
}

TimerId Reactor::runAt(const Date& time, Func&& cb)
{
	auto microSeconds =
		time.microSecondsSinceEpoch() - Date::now().microSecondsSinceEpoch();
	std::chrono::steady_clock::time_point tp =
		std::chrono::steady_clock::now() +
		std::chrono::microseconds(microSeconds);
	return _timerQueue->addTimer(std::move(cb),
		tp,
		std::chrono::microseconds(0));
}

TimerId Reactor::runAfter(double delay, const Func& cb)
{
	return runAt(Date::date().after(delay), cb);
}

TimerId Reactor::runAfter(double delay, Func&& cb)
{
	return runAt(Date::date().after(delay), std::move(cb));
}

TimerId Reactor::runEvery(double interval, const Func& cb)
{
	std::chrono::microseconds dur(
		static_cast<std::chrono::microseconds::rep>(interval * 1000000));
	auto tp = std::chrono::steady_clock::now() + dur;
	return _timerQueue->addTimer(cb, tp, dur);
}

TimerId Reactor::runEvery(double interval, Func&& cb)
{
	std::chrono::microseconds dur(
		static_cast<std::chrono::microseconds::rep>(interval * 1000000));
	auto tp = std::chrono::steady_clock::now() + dur;
	return _timerQueue->addTimer(std::move(cb), tp, dur);
}

void Reactor::invalidateTimer(TimerId id)
{
	if (!_stop && _timerQueue)
		_timerQueue->invalidateTimer(id);
}

void Reactor::doRunInLoopFuncs()
{
	Func func;
	while (_funcs.dequeue(func))
	{
		func();
	}
}

bool Reactor::has(const Socket& socket) const
{
	return _pollSet.has(socket);
}

bool Reactor::isInLoopThread() const
{
	return _threadId == Thread::currentTid();
};

void Reactor::onTimeout()
{
	dispatch(_pTimeoutNotification);
}

void Reactor::onIdle()
{
	dispatch(_pIdleNotification);
}

void Reactor::onShutdown()
{
	dispatch(_pShutdownNotification);
}

void Reactor::onBusy()
{
}

void Reactor::dispatch(const Socket& socket, CoNotification* pNotification)
{
	NotifierPtr pNotifier = getNotifier(socket);
	if (!pNotifier) return;
	dispatch(pNotifier, pNotification);
}

void Reactor::dispatch(CoNotification* pNotification)
{
	std::vector<NotifierPtr> delegates;
	delegates.reserve(_handlers.size());
	for (EventHandlerMap::iterator it = _handlers.begin(); it != _handlers.end(); ++it)
		delegates.push_back(it->second);
	for (std::vector<NotifierPtr>::iterator it = delegates.begin(); it != delegates.end(); ++it)
	{
		dispatch(*it, pNotification);
	}
}

void Reactor::dispatch(NotifierPtr& pNotifier, CoNotification* pNotification)
{
	try
	{
		pNotifier->dispatch(pNotification);
	}
	catch (Exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (std::exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (...)
	{
		ErrorHandler::handle();
	}
}