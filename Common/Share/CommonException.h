#pragma once
#include <stdexcept>
class CommonException : public std::exception
	/// This is the base class for all exceptions defined
	/// in the Poco class library.
{
public:
	CommonException(const std::string& msg, int code = 0);
	/// Creates an exception.

	CommonException(const std::string& msg, const std::string& arg, int code = 0);
	/// Creates an exception.

	CommonException(const std::string& msg, const CommonException& nested, int code = 0);
	/// Creates an exception and stores a clone
	/// of the nested exception.

	CommonException(const CommonException& exc);
	/// Copy constructor.

	~CommonException() noexcept;
	/// Destroys the exception and deletes the nested exception.

	CommonException& operator = (const CommonException& exc);
	/// Assignment operator.

	virtual const char* name() const noexcept;
	/// Returns a static string describing the exception.

	virtual const char* className() const noexcept;
	/// Returns the name of the exception class.

	virtual const char* what() const noexcept;
	/// Returns a static string describing the exception.
	///
	/// Same as name(), but for compatibility with std::exception.

	const CommonException* nested() const;
	/// Returns a pointer to the nested exception, or
	/// null if no nested exception exists.

	const std::string& message() const;
	/// Returns the message text.

	int code() const;
	/// Returns the exception code if defined.

	std::string displayText() const;
	/// Returns a string consisting of the
	/// message name and the message text.

	virtual CommonException* clone() const;
	/// Creates an exact copy of the exception.
	///
	/// The copy can later be thrown again by
	/// invoking rethrow() on it.

	virtual void rethrow() const;
	/// (Re)Throws the exception.
	///
	/// This is useful for temporarily storing a
	/// copy of an exception (see clone()), then
	/// throwing it again.

protected:
	CommonException(int code = 0);
	/// Standard constructor.

	void message(const std::string& msg);
	/// Sets the message for the exception.

	void extendedMessage(const std::string& arg);
	/// Sets the extended message for the exception.

private:
	std::string          _msg;
	CommonException* _pNested;
	int			        _code;
};


//
// inlines
//
inline const CommonException* CommonException::nested() const
{
	return _pNested;
}


inline const std::string& CommonException::message() const
{
	return _msg;
}


inline void CommonException::message(const std::string& msg)
{
	_msg = msg;
}


inline int CommonException::code() const
{
	return _code;
}

//
// Macros for quickly declaring and implementing exception classes.
// Unfortunately, we cannot use a template here because character
// pointers (which we need for specifying the exception name)
// are not allowed as template arguments.
//
#define COMMON_DECLARE_EXCEPTION_CODE(CLS, BASE, CODE) \
class CLS: public BASE														    \
{																				\
public:																			\
	CLS(int code = CODE);														\
	CLS(const std::string& msg, int code = CODE);								\
	CLS(const std::string& msg, const std::string& arg, int code = CODE);		\
	CLS(const std::string& msg, const CommonException& exc, int code = CODE);	\
	CLS(const CLS& exc);														\
	~CLS() noexcept;															\
	CLS& operator = (const CLS& exc);											\
	const char* name() const noexcept;											\
	const char* className() const noexcept;										\
	CommonException* clone() const;												\
	void rethrow() const;														\
};

#define COMMON_DECLARE_EXCEPTION(CLS, BASE) \
COMMON_DECLARE_EXCEPTION_CODE(CLS, BASE, 0)

#define COMMON_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)												\
CLS::CLS(int code): BASE(code)																	\
{																								\
}																								\
CLS::CLS(const std::string& msg, int code): BASE(msg, code)										\
{																								\
}																								\
CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code)		\
{																								\
}																								\
CLS::CLS(const std::string& msg, const CommonException& exc, int code): BASE(msg, exc, code)	\
{																								\
}																								\
CLS::CLS(const CLS& exc): BASE(exc)																\
{																								\
}																								\
CLS::~CLS() noexcept																			\
{																								\
}																								\
CLS& CLS::operator = (const CLS& exc)															\
{																								\
	BASE::operator = (exc);																		\
	return *this;																				\
}																								\
const char* CLS::name() const noexcept															\
{																								\
	return NAME;																				\
}																								\
const char* CLS::className() const noexcept														\
{																								\
	return typeid(*this).name();																\
}																								\
CommonException* CLS::clone() const																\
{																								\
	return new CLS(*this);																		\
}																								\
void CLS::rethrow() const																		\
{																								\
	throw *this;																				\
}


COMMON_DECLARE_EXCEPTION(LuaEngineException, CommonException)


