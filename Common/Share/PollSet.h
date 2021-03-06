#pragma once
#include "Poco/Net/Socket.h"

#include <map>

using Poco::Net::Socket;

class PollSetImpl;


class PollSet
	/// A set of sockets that can be efficiently polled as a whole.
	///
	/// If supported, PollSet is implemented using epoll (Linux) or
	/// poll (BSD) APIs. A fallback implementation using select()
	/// is also provided.
{
public:
	enum Mode
	{
		POLL_READ = 0x01,
		POLL_WRITE = 0x02,
		POLL_ERROR = 0x04
	};

	using SocketModeMap = std::map<Poco::Net::Socket, int>;

	PollSet();
	/// Creates an empty PollSet.

	~PollSet();
	/// Destroys the PollSet.

	void add(const Poco::Net::Socket& socket, int mode);
	/// Adds the given socket to the set, for polling with
	/// the given mode, which can be an OR'd combination of
	/// POLL_READ, POLL_WRITE and POLL_ERROR.

	void remove(const Poco::Net::Socket& socket);
	/// Removes the given socket from the set.

	void update(const Poco::Net::Socket& socket, int mode);
	/// Updates the mode of the given socket.

	bool has(const Socket& socket) const;
	/// Returns true if socket is registered for polling.

	bool empty() const;
	/// Returns true if no socket is registered for polling.

	void clear();
	/// Removes all sockets from the PollSet.

	SocketModeMap poll(const Poco::Timespan& timeout);
	/// Waits until the state of at least one of the PollSet's sockets
	/// changes accordingly to its mode, or the timeout expires.
	/// Returns a PollMap containing the sockets that have had
	/// their state changed.

	void postEvent();

private:
	PollSetImpl* _pImpl;

	PollSet(const PollSet&);
	PollSet& operator = (const PollSet&);
};


