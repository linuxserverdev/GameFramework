#include "Poco/String.h"
#include "Poco/Format.h"
#include "Poco/Tuple.h"
#include "Poco/NamedTuple.h"
#include "Poco/Exception.h"
#include "Poco/Data/LOB.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/MySQL/Utility.h"
#include "Poco/Data/MySQL/MySQLException.h"
#include "Poco/Nullable.h"
#include "Poco/Data/DataException.h"
#include <iostream>


using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::MySQL::ConnectionException;
using Poco::Data::MySQL::Utility;
using Poco::Data::MySQL::StatementException;
using Poco::format;
using Poco::NotFoundException;
using Poco::Int32;
using Poco::Nullable;
using Poco::Tuple;
using Poco::NamedTuple;

#define MYSQL_USER "root"
#define MYSQL_PWD  "root"
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_PORT 3306
#define MYSQL_DB   "pocotest"


void dbInfo(Session& session)
{
	std::cout << "Server Info: " << Utility::serverInfo(session) << std::endl;
	std::cout << "Server Version: " << Utility::serverVersion(session) << std::endl;
	std::cout << "Host Info: " << Utility::hostInfo(session) << std::endl;
}

void connectNoDB()
{
	std::string dbConnString = "host=" MYSQL_HOST
		";user=" MYSQL_USER
		";password=" MYSQL_PWD
		";compress=true;auto-reconnect=true;protocol=tcp";

	try
	{
		Session session(MySQL::Connector::KEY, dbConnString);
		std::cout << "Connected to [" << "MySQL" << "] without database." << std::endl;
		dbInfo(session);
		session << "CREATE DATABASE IF NOT EXISTS " MYSQL_DB ";", now;
		std::cout << "Disconnecting ..." << std::endl;
		session.close();
		std::cout << "Disconnected." << std::endl;
	}
	catch (ConnectionFailedException& ex)
	{
		std::cout << ex.displayText() << std::endl;
	}
}

int main()
{
	MySQL::Connector::registerConnector();
	connectNoDB();
	MySQL::Connector::unregisterConnector();
	return 0;
}