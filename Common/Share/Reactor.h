#pragma once
#include "Hpp/LockFreeQueue.h"
#include "Share/TimerQueue.h"
#include "Share/Date.h"
#include "Share/CoNotification.h"
#include "Share/CoNotifier.h"
#include "Share/PollSet.h"

#include "Poco/Net/Net.h"
#include "Poco/Net/Socket.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/Timespan.h"
#include "Poco/Observer.h"
#include "Poco/AutoPtr.h"
#include <map>
#include <atomic>

using Poco::Thread;
using Poco::Net::Socket;

using Func = std::function<void()>;

class Reactor : public Poco::Runnable
	/// This class, which is part of the Reactor pattern,
	/// implements the "Initiation Dispatcher".
	///
	/// The Reactor pattern has been described in the book
	/// "Pattern Languages of Program Design" by Jim Coplien
	/// and Douglas C. Schmidt (Addison Wesley, 1995).
	///
	/// The Reactor design pattern handles service requests that
	/// are delivered concurrently to an application by one or more
	/// clients. Each service in an application may consist of several
	/// methods and is represented by a separate event handler. The event
	/// handler is responsible for servicing service-specific requests.
	/// The Reactor dispatches the event handlers.
	///
	/// Event handlers (any class can be an event handler - there
	/// is no base class for event handlers) can be registered
	/// with the addEventHandler() method and deregistered with
	/// the removeEventHandler() method.
	/// 
	/// An event handler is always registered for a certain socket,
	/// which is given in the call to addEventHandler(). Any method
	/// of the event handler class can be registered to handle the
	/// event - the only requirement is that the method takes
	/// a pointer to an instance of SocketNotification (or a subclass of it)
	/// as argument.
	///
	/// Once started, the Reactor waits for events
	/// on the registered sockets, using Socket::select().
	/// If an event is detected, the corresponding event handler
	/// is invoked. There are five event types (and corresponding
	/// notification classes) defined: ReadableNotification, WritableNotification,
	/// ErrorNotification, TimeoutNotification, IdleNotification and 
	/// ShutdownNotification.
	/// 
	/// The ReadableNotification will be dispatched if a socket becomes
	/// readable. The WritableNotification will be dispatched if a socket
	/// becomes writable. The ErrorNotification will be dispatched if
	/// there is an error condition on a socket.
	///
	/// If the timeout expires and no event has occurred, a
	/// TimeoutNotification will be dispatched to all event handlers
	/// registered for it. This is done in the onTimeout() method
	/// which can be overridden by subclasses to perform custom
	/// timeout processing.
	///
	/// If there are no sockets for the Reactor to pass to
	/// Socket::select(), an IdleNotification will be dispatched to
	/// all event handlers registered for it. This is done in the
	/// onIdle() method which can be overridden by subclasses
	/// to perform custom idle processing. Since onIdle() will be
	/// called repeatedly in a loop, it is recommended to do a
	/// short sleep or yield in the event handler.
	///
	/// Finally, when the Reactor is about to shut down (as a result 
	/// of stop() being called), it dispatches a ShutdownNotification
	/// to all event handlers. This is done in the onShutdown() method
	/// which can be overridded by subclasses to perform custom
	/// shutdown processing.
	///
	/// The Reactor is implemented so that it can 
	/// run in its own thread. It is also possible to run
	/// multiple SocketReactors in parallel, as long as
	/// they work on different sockets.
	///
	/// It is safe to call addEventHandler() and removeEventHandler()
	/// from another thread while the Reactor is running. Also,
	/// it is safe to call addEventHandler() and removeEventHandler()
	/// from event handlers.
{
public:
	Reactor();
	/// Creates the Reactor.

	explicit Reactor(const Poco::Timespan& timeout);
	/// Creates the Reactor, using the given timeout.

	virtual ~Reactor();
	/// Destroys the Reactor.

	void run();
	/// Runs the Reactor. The reactor will run
	/// until stop() is called (in a separate thread).

	void stop();
	/// Stops the Reactor.
	///
	/// The reactor will be stopped when the next event
	/// (including a timeout event) occurs.

	void addEventHandler(const Socket& socket, const Poco::AbstractObserver& observer);
	/// Registers an event handler with the Reactor.
	///
	/// Usage:
	///     Poco::Observer<MyEventHandler, SocketNotification> obs(*this, &MyEventHandler::handleMyEvent);
	///     reactor.addEventHandler(obs);

	bool hasEventHandler(const Socket& socket, const Poco::AbstractObserver& observer);
	/// Returns true if the observer is registered with Reactor for the given socket.

	void removeEventHandler(const Socket& socket, const Poco::AbstractObserver& observer);
	/// Unregisters an event handler with the Reactor.
	///
	/// Usage:
	///     Poco::Observer<MyEventHandler, SocketNotification> obs(*this, &MyEventHandler::handleMyEvent);
	///     reactor.removeEventHandler(obs);


	void abortNotInLoopThread();
	void assertInLoopThread();

	void runInLoop(const Func& cb);
	void runInLoop(Func&& cb);

	TimerId runAt(const Date& time, const Func& cb);
	TimerId runAt(const Date& time, Func&& cb);
	TimerId runAfter(double delay, const Func& cb);
	TimerId runAfter(double delay, Func&& cb);
	TimerId runEvery(double interval, const Func& cb);
	TimerId runEvery(double interval, Func&& cb);
	void invalidateTimer(TimerId id);
	void doRunInLoopFuncs();

	bool has(const Socket& socket) const;
	/// Returns true if socket is registered with this rector.

	bool isInLoopThread() const;

protected:
	virtual void onTimeout();
	/// Called if the timeout expires and no other events are available.
	///
	/// Can be overridden by subclasses. The default implementation
	/// dispatches the TimeoutNotification and thus should be called by overriding
	/// implementations.

	virtual void onIdle();
	/// Called if no sockets are available to call select() on.
	///
	/// Can be overridden by subclasses. The default implementation
	/// dispatches the IdleNotification and thus should be called by overriding
	/// implementations.

	virtual void onShutdown();
	/// Called when the Reactor is about to terminate.
	///
	/// Can be overridden by subclasses. The default implementation
	/// dispatches the ShutdownNotification and thus should be called by overriding
	/// implementations.

	virtual void onBusy();
	/// Called when the Reactor is busy and at least one notification
	/// has been dispatched.
	///
	/// Can be overridden by subclasses to perform additional
	/// periodic tasks. The default implementation does nothing.

	void dispatch(const Socket& socket, CoNotification* pNotification);
	/// Dispatches the given notification to all observers
	/// registered for the given socket.

	void dispatch(CoNotification* pNotification);
	/// Dispatches the given notification to all observers.

private:
	typedef Poco::AutoPtr<CoNotifier>         NotifierPtr;
	typedef Poco::AutoPtr<CoNotification>     NotificationPtr;
	typedef std::map<Socket, NotifierPtr>     EventHandlerMap;
	typedef Poco::FastMutex                   MutexType;
	typedef MutexType::ScopedLock             ScopedLock;

	bool hasSocketHandlers();
	void dispatch(NotifierPtr& pNotifier, CoNotification* pNotification);
	NotifierPtr getNotifier(const Socket& socket, bool makeNew = false);

	enum
	{
		DEFAULT_TIMEOUT = 250000
	};

	std::atomic<bool>           _stop;
	Poco::Timespan              _timeout;
	EventHandlerMap             _handlers;
	PollSet                     _pollSet;
	NotificationPtr             _pReadableNotification;
	NotificationPtr             _pWritableNotification;
	NotificationPtr             _pErrorNotification;
	NotificationPtr             _pTimeoutNotification;
	NotificationPtr             _pIdleNotification;
	NotificationPtr             _pShutdownNotification;
	MpscQueue<Func>             _funcs;
	std::unique_ptr<TimerQueue> _timerQueue;
	Poco::Thread::TID           _threadId;

	friend class CoNotifier;
};



