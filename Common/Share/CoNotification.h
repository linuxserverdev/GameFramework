#pragma once

#include "Poco/Net/Net.h"
#include "Poco/Net/Socket.h"
#include "Poco/Notification.h"


class Reactor;

using Poco::Net::Socket;

class CoNotification : public Poco::Notification
	/// The base class for all notifications generated by
	/// the SocketReactor.
{
public:
	explicit CoNotification(Reactor* pReactor);
	/// Creates the CoNotification for the given Reactor.

	virtual ~CoNotification();
	/// Destroys the CoNotification.

	Reactor& source() const;
	/// Returns the Reactor that generated the notification.	

	Socket socket() const;
	/// Returns the socket that caused the notification.

private:
	void setSocket(const Socket& socket);

	Reactor* _pReactor;
	Socket   _socket;

	friend class CoNotifier;
};


class  ReadableNotification : public CoNotification
	/// This notification is sent if a socket has become readable.
{
public:
	ReadableNotification(Reactor* pReactor);
	/// Creates the ReadableNotification for the given Reactor.

	~ReadableNotification();
	/// Destroys the ReadableNotification.
};


class  WritableNotification : public CoNotification
	/// This notification is sent if a socket has become writable.
{
public:
	WritableNotification(Reactor* pReactor);
	/// Creates the WritableNotification for the given Reactor.

	~WritableNotification();
	/// Destroys the WritableNotification.
};


class  ErrorNotification : public CoNotification
	/// This notification is sent if a socket has signalled an error.
{
public:
	ErrorNotification(Reactor* pReactor);
	/// Creates the ErrorNotification for the given Reactor.

	~ErrorNotification();
	/// Destroys the ErrorNotification.
};


class  TimeoutNotification : public CoNotification
	/// This notification is sent if no other event has occurred
	/// for a specified time.
{
public:
	TimeoutNotification(Reactor* pReactor);
	/// Creates the TimeoutNotification for the given Reactor.

	~TimeoutNotification();
	/// Destroys the TimeoutNotification.
};


class  IdleNotification : public CoNotification
	/// This notification is sent when the Reactor does
	/// not have any sockets to react to.
{
public:
	IdleNotification(Reactor* pReactor);
	/// Creates the IdleNotification for the given Reactor.

	~IdleNotification();
	/// Destroys the IdleNotification.
};


class  ShutdownNotification : public CoNotification
	/// This notification is sent when the Reactor is
	/// about to shut down.
{
public:
	ShutdownNotification(Reactor* pReactor);
	/// Creates the ShutdownNotification for the given Reactor.

	~ShutdownNotification();
	/// Destroys the ShutdownNotification.
};


//
// inlines
//
inline Reactor& CoNotification::source() const
{
	return *_pReactor;
}

inline Socket CoNotification::socket() const
{
	return _socket;
}