#include "CommonException.h"
#include <typeinfo>


CommonException::CommonException(int code) : _pNested(0), _code(code)
{
}


CommonException::CommonException(const std::string& msg, int code) : _msg(msg), _pNested(0), _code(code)
{
}


CommonException::CommonException(const std::string& msg, const std::string& arg, int code) : _msg(msg), _pNested(0), _code(code)
{
	if (!arg.empty())
	{
		_msg.append(": ");
		_msg.append(arg);
	}
}


CommonException::CommonException(const std::string& msg, const CommonException& nested, int code) : _msg(msg), _pNested(nested.clone()), _code(code)
{
}


CommonException::CommonException(const CommonException& exc) :
	std::exception(exc),
	_msg(exc._msg),
	_code(exc._code)
{
	_pNested = exc._pNested ? exc._pNested->clone() : 0;
}


CommonException::~CommonException() noexcept
{
	delete _pNested;
}


CommonException& CommonException::operator = (const CommonException& exc)
{
	if (&exc != this)
	{
		CommonException* newPNested = exc._pNested ? exc._pNested->clone() : 0;
		delete _pNested;
		_msg = exc._msg;
		_pNested = newPNested;
		_code = exc._code;
	}
	return *this;
}


const char* CommonException::name() const noexcept
{
	return "CommonException";
}


const char* CommonException::className() const noexcept
{
	return typeid(*this).name();
}


const char* CommonException::what() const noexcept
{
	return name();
}


std::string CommonException::displayText() const
{
	std::string txt = name();
	if (!_msg.empty())
	{
		txt.append(": ");
		txt.append(_msg);
	}
	return txt;
}


void CommonException::extendedMessage(const std::string& arg)
{
	if (!arg.empty())
	{
		if (!_msg.empty()) _msg.append(": ");
		_msg.append(arg);
	}
}


CommonException* CommonException::clone() const
{
	return new CommonException(*this);
}


void CommonException::rethrow() const
{
	throw* this;
}


COMMON_IMPLEMENT_EXCEPTION(LuaEngineException, CommonException, "Lua Engine Exception")
