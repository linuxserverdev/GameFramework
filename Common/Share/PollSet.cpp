#include "Share/PollSet.h"
#include "Poco/Net/SocketImpl.h"
#include "Poco/Mutex.h"
#include <set>

using Poco::Net::SocketImpl;

#ifdef __linux__
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h>
#include <iostream>
#elif defined _WIN32
#include "Share/Wepoll.h"
#include <assert.h>
#include <iostream>
#include <winsock2.h>
#include <fcntl.h>
#endif

#ifdef __linux__
int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		std::cout << "Failed in eventfd" << std::endl;
		abort();
	}

	return evtfd;
}
#endif

class PollSetImpl
{
public:
	PollSetImpl() :
		_events(1024)
	{
#ifdef __linux__
		_platformBits = 0;
		if (sizeof(void*) == 4)
		{
			_platformBits = 32;
		}
		else if (sizeof(void*) == 8)
		{
			_platformBits = 64;
		}
		_wakeupFd = createEventfd();
#endif 	
		create();
	}

	~PollSetImpl()
	{
		close();
	}

	void add(const Socket& socket, int mode)
	{
		SocketImpl* sockImpl = socket.impl();
		poco_socket_t fd = sockImpl->sockfd();
		struct epoll_event ev;
		ev.events = 0;
		if (mode & PollSet::POLL_READ)
			ev.events |= EPOLLIN;
		if (mode & PollSet::POLL_WRITE)
			ev.events |= EPOLLOUT;
		if (mode & PollSet::POLL_ERROR)
			ev.events |= EPOLLERR;
		ev.data.ptr = socket.impl();
		int err = epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev);

		if (err)
		{
			if (errno == EEXIST) update(socket, mode);
			else SocketImpl::error();
		}

		if (_socketMap.find(sockImpl) == _socketMap.end())
			_socketMap[sockImpl] = socket;
	}

#ifdef __linux__
	void addEventFd()
	{
		struct epoll_event ev;
		ev.events = 0;
		ev.events |= EPOLLIN;	
		if (_platformBits == 32)
		{
			ev.data.ptr = (void*)(reinterpret_cast<uint32_t>(0x80000000));
		}
		else if (_platformBits == 64)
		{
			ev.data.ptr = (void*)(reinterpret_cast<uint64_t>(0x8000000000000000));
		}
		int err = epoll_ctl(_epollfd, EPOLL_CTL_ADD, _wakeupFd, &ev);

		if (err)
		{
			SocketImpl::error();
		}
	}
#endif
	void remove(const Socket& socket)
	{
		poco_socket_t fd = socket.impl()->sockfd();
		struct epoll_event ev;
		ev.events = 0;
		ev.data.ptr = 0;
		int err = epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &ev);
		if (err) SocketImpl::error();

		_socketMap.erase(socket.impl());
	}

	bool has(const Socket& socket) const
	{
		SocketImpl* sockImpl = socket.impl();
		return sockImpl &&
			(_socketMap.find(sockImpl) != _socketMap.end());
	}

	bool empty() const
	{
		return _socketMap.empty();
	}

	void update(const Socket& socket, int mode)
	{
		poco_socket_t fd = socket.impl()->sockfd();
		struct epoll_event ev;
		ev.events = 0;
		if (mode & PollSet::POLL_READ)
			ev.events |= EPOLLIN;
		if (mode & PollSet::POLL_WRITE)
			ev.events |= EPOLLOUT;
		if (mode & PollSet::POLL_ERROR)
			ev.events |= EPOLLERR;
		ev.data.ptr = socket.impl();
		int err = epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev);
		if (err)
		{
			SocketImpl::error();
		}
	}

	void clear()
	{
		close();
		_socketMap.clear();
		create();
	}

	PollSet::SocketModeMap poll(const Poco::Timespan& timeout)
	{
		PollSet::SocketModeMap result;

		if (_socketMap.empty()) return result;

		Poco::Timespan remainingTime(timeout);
		int rc;
		do
		{
			Poco::Timestamp start;
			rc = epoll_wait(_epollfd, &_events[0], _events.size(), remainingTime.totalMilliseconds());
			if (rc < 0 && SocketImpl::lastError() == POCO_EINTR)
			{
				Poco::Timestamp end;
				Poco::Timespan waited = end - start;
				if (waited < remainingTime)
					remainingTime -= waited;
				else
					remainingTime = 0;
			}
		} while (rc < 0 && SocketImpl::lastError() == POCO_EINTR);
		if (rc < 0) SocketImpl::error();


		for (int i = 0; i < rc; i++)
		{
			std::map<void*, Socket>::iterator it = _socketMap.find(_events[i].data.ptr);
			if (it != _socketMap.end())
			{
				if (_events[i].events & EPOLLIN)
					result[it->second] |= PollSet::POLL_READ;
				if (_events[i].events & EPOLLOUT)
					result[it->second] |= PollSet::POLL_WRITE;
				if (_events[i].events & EPOLLERR)
					result[it->second] |= PollSet::POLL_ERROR;
			}
			else
			{
#ifdef __linux__
				if ((_platformBits == 32 && (reinterpret_cast<uint32_t>(_events[i].data.ptr) & 0x80000000))
					|| (_platformBits == 64 && (reinterpret_cast<uint32_t>(_events[i].data.ptr) & 0x8000000000000000))
				{
					ssize_t ret = 0;
						uint64_t tmp;
						ret = read(_wakeupFd, &tmp, sizeof(tmp));

						if (ret < 0)
							LOG_SYSERR << "wakeup read error";
				}
#endif
			}
		}

		return result;
	}

	void postEvent()
	{
		uint64_t tmp = 1;
#ifdef __linux__
		int ret = write(_wakeupFd, &tmp, sizeof(tmp));
#elif defined _WIN32
		epoll_post_signal(_epollfd, tmp);
#endif
	}

private:
	void create()
	{
#ifdef _WIN32
		// wepoll does not suppor flags
		_epollfd = ::epoll_create1(0);
		if (_epollfd == 0)
		{
			SocketImpl::error();
		}
#else
		_epollfd = ::epoll_create1(EPOLL_CLOEXEC);
		if (_epollfd < 0)
		{
			SocketImpl::error();
		}
#endif
	}
	void close()
	{
#ifdef _WIN32
		epoll_close(_epollfd);
#else
		close(_epollfd);
#endif
	}
private:
#ifdef _WIN32
	void*                           _epollfd;
#else
	int                             _epollfd;
#endif
	std::map<void*, Socket>         _socketMap;
	std::vector<struct epoll_event> _events;
#ifdef __linux__
	int                             _platformBits;
	int                             _wakeupFd;
#endif // __linux__
};



PollSet::PollSet() :
	_pImpl(new PollSetImpl)
{
}


PollSet::~PollSet()
{
	delete _pImpl;
}


void PollSet::add(const Socket& socket, int mode)
{
	_pImpl->add(socket, mode);
}


void PollSet::remove(const Socket& socket)
{
	_pImpl->remove(socket);
}


void PollSet::update(const Socket& socket, int mode)
{
	_pImpl->update(socket, mode);
}


bool PollSet::has(const Socket& socket) const
{
	return _pImpl->has(socket);
}


bool PollSet::empty() const
{
	return _pImpl->empty();
}


void PollSet::clear()
{
	_pImpl->clear();
}


PollSet::SocketModeMap PollSet::poll(const Poco::Timespan& timeout)
{
	return _pImpl->poll(timeout);
}

void PollSet::postEvent()
{
	_pImpl->postEvent();
}
