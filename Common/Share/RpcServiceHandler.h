#pragma once

#include "Share/MsgBuffer.h"
#include "Share/RpcMsgParser.h"
#include "Share/RpcController.h"
#include "Share/Reactor.h"
#include "Share/CoNotification.h"
#include "Logger/Logger.h"

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Observer.h"

using Poco::Net::StreamSocket;
using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Observer;


template <class ServiceHandler>
class Listener
	/// This class implements the Acceptor part of the
	/// Acceptor-Connector design pattern.
	///
	/// The Acceptor-Connector pattern has been described in the book
	/// "Pattern Languages of Program Design 3", edited by Robert Martin,
	/// Frank Buschmann and Dirk Riehle (Addison Wesley, 1997).
	///
	/// The Acceptor-Connector design pattern decouples connection
	/// establishment and service initialization in a distributed system
	/// from the processing performed once a service is initialized.
	/// This decoupling is achieved with three components: Acceptors, 
	/// Connectors and Service Handlers.
	/// The Listener passively waits for connection requests (usually
	/// from a remote Connector) and establishes a connection upon
	/// arrival of a connection requests. Also, a Service Handler is
	/// initialized to process the data arriving via the connection in
	/// an application-specific way.
	///
	/// The Listener sets up a ServerSocket and registers itself
	/// for a ReadableNotification, denoting an incoming connection request.
	///
	/// When the ServerSocket becomes readable the Listener accepts
	/// the connection request and creates a ServiceHandler to
	/// service the connection.
	///
	/// The ServiceHandler class must provide a constructor that
	/// takes a StreamSocket and a Reactor as arguments,
	/// e.g.:
	///     MyServiceHandler(const StreamSocket& socket, ServiceReactor& reactor)
	///
	/// When the ServiceHandler is done, it must destroy itself.
	///
	/// Subclasses can override the createServiceHandler() factory method
	/// if special steps are necessary to create a ServiceHandler object.
{
public:
	using Observer = Poco::Observer<Listener, ReadableNotification>;

	explicit Listener(ServerSocket& socket, RpcType tp) :
		_socket(socket),
		_pReactor(0),
		_type(tp)
		/// Creates a Listener, using the given ServerSocket.
	{
	}

	Listener(ServerSocket& socket, Reactor& reactor, RpcType tp) :
		_socket(socket),
		_pReactor(&reactor),
		_type(tp)
		/// Creates a Listener, using the given ServerSocket.
		/// The Listener registers itself with the given Reactor.
	{
		_pReactor->addEventHandler(_socket, Observer(*this, &Listener::onAccept));
	}

	virtual ~Listener()
		/// Destroys the Listener.
	{
		try
		{
			if (_pReactor)
			{
				_pReactor->removeEventHandler(_socket, Observer(*this, &Listener::onAccept));
			}
		}
		catch (...)
		{
			poco_unexpected();
		}
	}

	void setReactor(Reactor& reactor)
		/// Sets the reactor for this acceptor.
	{
		registerAcceptor(reactor);
	}

	virtual void registerAcceptor(Reactor& reactor)
		/// Registers the Listener with a Reactor.
		///
		/// A subclass can override this function to e.g.
		/// register an event handler for timeout event.
		/// 
		/// If acceptor was constructed without providing reactor to it,
		/// the override of this method must either call the base class
		/// implementation or directly register the accept handler with
		/// the reactor.
	{
		_pReactor = &reactor;
		if (!_pReactor->hasEventHandler(_socket, Observer(*this, &Listener::onAccept)))
		{
			_pReactor->addEventHandler(_socket, Observer(*this, &Listener::onAccept));
		}
	}

	virtual void unregisterAcceptor()
		/// Unregisters the Listener.
		///
		/// A subclass can override this function to e.g.
		/// unregister its event handler for a timeout event.
		/// 
		/// If the accept handler was registered with the reactor,
		/// the overriding method must either call the base class
		/// implementation or directly unregister the accept handler.
	{
		if (_pReactor)
		{
			_pReactor->removeEventHandler(_socket, Observer(*this, &Listener::onAccept));
		}
	}

	void onAccept(ReadableNotification* pNotification)
		/// Accepts connection and creates event handler.
	{
		pNotification->release();
		StreamSocket sock = _socket.acceptConnection();
		_pReactor->wakeUp();
		createServiceHandler(sock);
	}

protected:
	virtual ServiceHandler* createServiceHandler(StreamSocket& socket)
		/// Create and initialize a new ServiceHandler instance.
		///
		/// Subclasses can override this method.
	{
		return new ServiceHandler(socket, *(this->reactor()), _type);
	}

	Reactor* reactor()
		/// Returns a pointer to the Reactor where
		/// this Listener is registered.
		///
		/// The pointer may be null.
	{
		return _pReactor;
	}

	Socket& socket()
		/// Returns a reference to the Listener's socket.
	{
		return _socket;
	}

private:
	Listener();
	Listener(const Listener&);
	Listener& operator = (const Listener&);

	ServerSocket   _socket;
	Reactor*       _pReactor;
	RpcType        _type;
};


template <class ServiceHandler>
class Connector
	/// This class implements the Connector part of the
	/// Acceptor-Connector design pattern.
	///
	/// The Acceptor-Connector pattern has been described in the book
	/// "Pattern Languages of Program Design 3", edited by Robert Martin,
	/// Frank Buschmann and Dirk Riehle (Addison Wesley, 1997).
	///
	/// The Acceptor-Connector design pattern decouples connection
	/// establishment and service initialization in a distributed system
	/// from the processing performed once a service is initialized.
	/// This decoupling is achieved with three components: Acceptors, 
	/// Connectors and Service Handlers.
	/// The Connector actively establishes a connection with a remote
	/// server socket (usually managed by an Acceptor) and initializes
	/// a Service Handler to manage the connection.
	///
	/// The Connector sets up a StreamSocket, initiates a non-blocking
	/// connect operation and registers itself for ReadableNotification, WritableNotification
	/// and ErrorNotification. ReadableNotification or WritableNotification denote the successful 
	/// establishment of the connection.
	///
	/// When the StreamSocket becomes readable or writeable, the Connector 
	/// creates a ServiceHandler to service the connection and unregisters
	/// itself.
	///
	/// In case of an error (ErrorNotification), the Connector unregisters itself
	/// and calls the onError() method, which can be overridden by subclasses
	/// to perform custom error handling.
	///
	/// The ServiceHandler class must provide a constructor that
	/// takes a StreamSocket and a Reactor as arguments,
	/// e.g.:
	///     MyServiceHandler(const StreamSocket& socket, Reactor& reactor)
	///
	/// When the ServiceHandler is done, it must destroy itself.
	///
	/// Subclasses can override the createServiceHandler() factory method
	/// if special steps are necessary to create a ServiceHandler object.
{
public:
	explicit Connector(SocketAddress& address, RpcType tp) :
		_pReactor(0),
		_type(tp)
		/// Creates a Connector, using the given Socket.
	{
		_socket.connectNB(address);
	}

	Connector(SocketAddress& address, Reactor& reactor, RpcType tp, bool doRegister = true) :
		_pReactor(0),
		_type(tp)
		/// Creates an acceptor, using the given ServerSocket.
		/// The Connector registers itself with the given Reactor.
	{
		_socket.connectNB(address);
		if (doRegister) registerConnector(reactor);
	}

	virtual ~Connector()
		/// Destroys the Connector.
	{
		try
		{
			unregisterConnector();
		}
		catch (...)
		{
			poco_unexpected();
		}
	}

	virtual void registerConnector(Reactor& reactor)
		/// Registers the Connector with a Reactor.
		///
		/// A subclass can override this and, for example, also register
		/// an event handler for a timeout event.
		///
		/// The overriding method must call the baseclass implementation first.
	{
		_pReactor = &reactor;
		_pReactor->addEventHandler(_socket, Poco::Observer<Connector, ReadableNotification>(*this, &Connector::onReadable));
		_pReactor->addEventHandler(_socket, Poco::Observer<Connector, WritableNotification>(*this, &Connector::onWritable));
		_pReactor->addEventHandler(_socket, Poco::Observer<Connector, ErrorNotification>(*this, &Connector::onError));
	}

	virtual void unregisterConnector()
		/// Unregisters the Connector.
		///
		/// A subclass can override this and, for example, also unregister
		/// its event handler for a timeout event.
		///
		/// The overriding method must call the baseclass implementation first.
	{
		if (_pReactor)
		{
			_pReactor->removeEventHandler(_socket, Poco::Observer<Connector, ReadableNotification>(*this, &Connector::onReadable));
			_pReactor->removeEventHandler(_socket, Poco::Observer<Connector, WritableNotification>(*this, &Connector::onWritable));
			_pReactor->removeEventHandler(_socket, Poco::Observer<Connector, ErrorNotification>(*this, &Connector::onError));
		}
	}

	void onReadable(ReadableNotification* pNotification)
	{
		pNotification->release();
		int err = _socket.impl()->socketError();
		if (err)
		{
			onError(err);
			unregisterConnector();
		}
		else
		{
			onConnect();
		}
	}

	void onWritable(WritableNotification* pNotification)
	{
		pNotification->release();
		onConnect();
	}

	void onConnect()
	{
		_socket.setBlocking(true);
		createServiceHandler();
		unregisterConnector();
	}

	void onError(ErrorNotification* pNotification)
	{
		pNotification->release();
		onError(_socket.impl()->socketError());
		unregisterConnector();
	}

protected:
	virtual ServiceHandler* createServiceHandler()
		/// Create and initialize a new ServiceHandler instance.
		///
		/// Subclasses can override this method.
	{
		return new ServiceHandler(this->socket(), *(this->reactor()), _type);
	}

	virtual void onError(int errorCode)
		/// Called when the socket cannot be connected.
		///
		/// Subclasses can override this method.
	{
		LOG_ERROR << RpcMsgPacket::getRpcTypeString(_type) << " connect err, errorCode = " << errorCode;
	}

	Reactor* reactor()
		/// Returns a pointer to the Reactor where
		/// this Connector is registered.
		///
		/// The pointer may be null.
	{
		return _pReactor;
	}

	StreamSocket& socket()
		/// Returns a reference to the Connector's socket.
	{
		return _socket;
	}

private:
	Connector();
	Connector(const Connector&);
	Connector& operator = (const Connector&);

	StreamSocket   _socket;
	Reactor*       _pReactor;
	RpcType        _type;
};


class RpcServiceHandler
{
public:
	RpcServiceHandler(const StreamSocket& socket, Reactor& reactor):
		_socket(socket),
		_reactor(reactor),
		_type(RPC_INVALID)
	{
		LOG_MASSERT(false, "delete constructor");
	}
	RpcServiceHandler(const StreamSocket& socket, Reactor& reactor, RpcType tp) :
		_socket(socket),
		_reactor(reactor),
		_type(tp)
	{
		LOG_INFO << RpcMsgPacket::getRpcTypeString(_type) << " new connection -> " << _socket.peerAddress().toString();
		_reactor.addEventHandler(_socket, Observer<RpcServiceHandler, ReadableNotification>(*this, &RpcServiceHandler::onReadable));
		_reactor.addEventHandler(_socket, Observer<RpcServiceHandler, WritableNotification>(*this, &RpcServiceHandler::onWritable));
		_reactor.addEventHandler(_socket, Observer<RpcServiceHandler, ShutdownNotification>(*this, &RpcServiceHandler::onShutdown));
		_reactor.addEventHandler(_socket, Observer<RpcServiceHandler, ErrorNotification>(*this, &RpcServiceHandler::onError));
	}

	~RpcServiceHandler()
	{
		LOG_INFO << RpcMsgPacket::getRpcTypeString(_type) << " close connection -> " << _socket.peerAddress().toString();
	}
	void onReadable(ReadableNotification* pNf)
	{
		pNf->release();
		int n = _recvBuffer.readStreamSocket(_socket);
		if (n > 0)							  
		{ 
			bool success = _parser.parse(&_recvBuffer);
			if (success)				 
			{ 		   
				MsgPtr msg;
				if (_parser.gotAll(msg))
				{
					RpcController::getIns().pushMsg(msg);
				}
				else return;
			}
			else
			{
				// error!
				removeEventHandler(); 
				delete this;
			}
		}
		else
		{
			removeEventHandler(); 
			delete this;
		}
	}
	void onWritable(WritableNotification* pNf)
	{
		pNf->release();
		std::string data(1024, 'x');
		_socket.sendBytes(data.data(), (int)data.length());
		_socket.shutdownSend();
	}
	void onError(ErrorNotification* pNf)
	{
		pNf->release();
		removeEventHandler();
		delete this;
	}
	void onShutdown(ShutdownNotification* pNf)
	{
		pNf->release();
		delete this;
	}

	void sendMsg(PacketPtr pk)
	{
		FastScopedLock lock(_sendMutex);
		_sendQueue.push_back(pk);
	}

	void removeWritable()
	{
		_reactor.removeEventHandler(_socket, Observer<RpcServiceHandler, WritableNotification>(*this, &RpcServiceHandler::onWritable));
	}

	void removeEventHandler()
	{
		_reactor.removeEventHandler(_socket, Observer<RpcServiceHandler, ReadableNotification>(*this, &RpcServiceHandler::onReadable));
		_reactor.removeEventHandler(_socket, Observer<RpcServiceHandler, ShutdownNotification>(*this, &RpcServiceHandler::onShutdown));
		_reactor.removeEventHandler(_socket, Observer<RpcServiceHandler, ErrorNotification>(*this, &RpcServiceHandler::onError));
	}
private:
	StreamSocket     _socket;
	Reactor&         _reactor;
	RpcType          _type;
	MsgBuffer        _recvBuffer;
	MsgBuffer        _sendBuffer;
	MsgQueue         _sendQueue;
	FastMutex        _sendMutex;
	RpcMsgParser     _parser;	
};