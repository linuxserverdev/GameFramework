
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/SocketConnector.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Observer.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include <sstream>
#include <assert.h>

using Poco::Net::SocketReactor;
using Poco::Net::SocketConnector;
using Poco::Net::SocketAcceptor;
using Poco::Net::ParallelSocketAcceptor;
using Poco::Net::StreamSocket;
using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SocketNotification;
using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::TimeoutNotification;
using Poco::Net::ShutdownNotification;
using Poco::Observer;
using Poco::IllegalStateException;
using Poco::Thread;



class ClientServiceHandler
{
public:
	ClientServiceHandler(const StreamSocket& socket, SocketReactor& reactor) :
		_socket(socket),
		_reactor(reactor),
		_or(*this, &ClientServiceHandler::onReadable),
		_ow(*this, &ClientServiceHandler::onWritable),
		_ot(*this, &ClientServiceHandler::onTimeout),
		_os(*this, &ClientServiceHandler::onShutdown)
	{
		_timeout = false;
		_readableError = false;
		_writableError = false;
		_timeoutError = false;
		checkReadableObserverCount(0);
		_reactor.addEventHandler(_socket, _or);
		checkReadableObserverCount(1);
		checkWritableObserverCount(0);
		_reactor.addEventHandler(_socket, _ow);
		checkWritableObserverCount(1);
		checkTimeoutObserverCount(0);
		_reactor.addEventHandler(_socket, _ot);
		checkTimeoutObserverCount(1);
		_reactor.addEventHandler(_socket, _os);
	}

	~ClientServiceHandler()
	{
		_reactor.removeEventHandler(_socket, _or);
		_reactor.removeEventHandler(_socket, _ow);
		_reactor.removeEventHandler(_socket, _ot);
		_reactor.removeEventHandler(_socket, _os);
	}

	void onReadable(ReadableNotification* pNf)
	{
		pNf->release();
		char buffer[32];
		int n = _socket.receiveBytes(buffer, sizeof(buffer));
		if (n > 0)
		{
			_str.write(buffer, n);
			_data += _str.str();
			_str.str("");
		}
		else
		{
			checkReadableObserverCount(1);
			_reactor.removeEventHandler(_socket, Observer<ClientServiceHandler, ReadableNotification>(*this, &ClientServiceHandler::onReadable));
			checkReadableObserverCount(0);
			if (_once || _data.size() == 8192)
			{
				_reactor.stop();
				delete this;
			}
		}
	}

	void onWritable(WritableNotification* pNf)
	{
		pNf->release();
		checkWritableObserverCount(1);
		_reactor.removeEventHandler(_socket, Observer<ClientServiceHandler, WritableNotification>(*this, &ClientServiceHandler::onWritable));
		checkWritableObserverCount(0);
		std::string data(1024, 'x');
		_socket.sendBytes(data.data(), (int)data.length());
		_socket.shutdownSend();
	}

	void onTimeout(TimeoutNotification* pNf)
	{
		pNf->release();
		_timeout = true;
		if (_closeOnTimeout)
		{
			_reactor.stop();
			delete this;
		}
	}

	void onShutdown(ShutdownNotification* pNf)
	{
		pNf->release();
		delete this;
	}

	static std::string data()
	{
		return _data;
	}

	static void resetData()
	{
		_data.clear();
	}

	static bool timeout()
	{
		return _timeout;
	}

	static bool getCloseOnTimeout()
	{
		return _closeOnTimeout;
	}

	static void setCloseOnTimeout(bool flag)
	{
		_closeOnTimeout = flag;
	}

	static bool readableError()
	{
		return _readableError;
	}

	static bool writableError()
	{
		return _writableError;
	}

	static bool timeoutError()
	{
		return _timeoutError;
	}

	static void setOnce(bool once = true)
	{
		_once = once;
	}

private:
	void checkReadableObserverCount(std::size_t oro)
	{
		if (((oro == 0) && _reactor.hasEventHandler(_socket, _or)) ||
			((oro > 0) && !_reactor.hasEventHandler(_socket, _or)))
		{
			_readableError = true;
		}
	}

	void checkWritableObserverCount(std::size_t ow)
	{
		if (((ow == 0) && _reactor.hasEventHandler(_socket, _ow)) ||
			((ow > 0) && !_reactor.hasEventHandler(_socket, _ow)))
		{
			_writableError = true;
		}
	}

	void checkTimeoutObserverCount(std::size_t ot)
	{
		if (((ot == 0) && _reactor.hasEventHandler(_socket, _ot)) ||
			((ot > 0) && !_reactor.hasEventHandler(_socket, _ot)))
		{
			_timeoutError = true;
		}
	}

	StreamSocket                                         _socket;
	SocketReactor& _reactor;
	Observer<ClientServiceHandler, ReadableNotification> _or;
	Observer<ClientServiceHandler, WritableNotification> _ow;
	Observer<ClientServiceHandler, TimeoutNotification>  _ot;
	Observer<ClientServiceHandler, ShutdownNotification> _os;
	std::stringstream                                    _str;
	static std::string                                   _data;
	static bool                                          _readableError;
	static bool                                          _writableError;
	static bool                                          _timeoutError;
	static bool                                          _timeout;
	static bool                                          _closeOnTimeout;
	static bool                                          _once;
};


std::string ClientServiceHandler::_data;
bool ClientServiceHandler::_readableError = false;
bool ClientServiceHandler::_writableError = false;
bool ClientServiceHandler::_timeoutError = false;
bool ClientServiceHandler::_timeout = false;
bool ClientServiceHandler::_closeOnTimeout = false;
bool ClientServiceHandler::_once = false;


class FailConnector : public SocketConnector<ClientServiceHandler>
{
public:
	FailConnector(SocketAddress& address, SocketReactor& reactor) :
		SocketConnector<ClientServiceHandler>(address, reactor),
		_failed(false),
		_shutdown(false)
	{
		reactor.addEventHandler(socket(), Observer<FailConnector, TimeoutNotification>(*this, &FailConnector::onTimeout));
		reactor.addEventHandler(socket(), Observer<FailConnector, ShutdownNotification>(*this, &FailConnector::onShutdown));
	}

	void onShutdown(ShutdownNotification* pNf)
	{
		pNf->release();
		_shutdown = true;
	}

	void onTimeout(TimeoutNotification* pNf)
	{
		pNf->release();
		_failed = true;
		reactor()->stop();
	}

	void onError(int error)
	{
		_failed = true;
		reactor()->stop();
	}

	bool failed() const
	{
		return _failed;
	}

	bool shutdown() const
	{
		return _shutdown;
	}

private:
	bool _failed;
	bool _shutdown;
};

class DataServiceHandler
{
public:
	typedef std::vector<std::string> Data;

	DataServiceHandler(StreamSocket& socket, SocketReactor& reactor) :
		_socket(socket),
		_reactor(reactor),
		_pos(0)
	{
		_data.resize(1);
		_reactor.addEventHandler(_socket, Observer<DataServiceHandler, ReadableNotification>(*this, &DataServiceHandler::onReadable));
		_reactor.addEventHandler(_socket, Observer<DataServiceHandler, ShutdownNotification>(*this, &DataServiceHandler::onShutdown));
	}

	~DataServiceHandler()
	{
		_reactor.removeEventHandler(_socket, Observer<DataServiceHandler, ReadableNotification>(*this, &DataServiceHandler::onReadable));
		_reactor.removeEventHandler(_socket, Observer<DataServiceHandler, ShutdownNotification>(*this, &DataServiceHandler::onShutdown));
	}

	void onReadable(ReadableNotification* pNf)
	{
		pNf->release();
		char buffer[64];
		int n = _socket.receiveBytes(&buffer[0], sizeof(buffer));
		if (n > 0)
		{
			_data[_pos].append(buffer, n);
			std::size_t pos;
			pos = _data[_pos].find('\n');
			if (pos != std::string::npos)
			{
				if (pos == _data[_pos].size() - 1)
				{
					_data[_pos].erase(pos, 1);
					_data.push_back(std::string());
				}
				else
				{
					_data.push_back(_data[_pos].substr(pos + 1));
					_data[_pos].erase(pos);
				}
				++_pos;
			}
		}
		else return;
	}

	void onShutdown(ShutdownNotification* pNf)
	{
		pNf->release();
		delete this;
	}

	static Data _data;

private:
	StreamSocket   _socket;
	SocketReactor& _reactor;
	int            _pos;
};

DataServiceHandler::Data DataServiceHandler::_data;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		return -1;
	}
	argv[1];
	int port = std::stoi(argv[1]);
	SocketReactor reactor;
	SocketAddress sa("127.0.0.1", port);
	SocketConnector<ClientServiceHandler> connector(sa, reactor);
	ClientServiceHandler::setOnce(true);
	ClientServiceHandler::resetData();
	reactor.run();
	std::string data(ClientServiceHandler::data());
	assert(data.size() == 1024);
	assert(!ClientServiceHandler::readableError());
	assert(!ClientServiceHandler::writableError());
	assert(!ClientServiceHandler::timeoutError());
	std::cout << data << std::endl;
	std::cin.get();
	return 0;
}