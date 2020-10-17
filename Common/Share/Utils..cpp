#include "Utils.h"
#include "CommonException.h"
#include <Logger/Logger.h>
#include <mutex>
#include <sstream>
#include <stack>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iomanip>
#ifdef _WIN32
#include <time.h>
#include <Rpc.h>
#include <direct.h>
#include <io.h>
#endif
#ifndef _WIN32
#include <cxxabi.h>
#endif

#ifdef _WIN32
char* strptime(const char* s, const char* f, struct tm* tm)
{
    // std::get_time is defined such that its
    // format parameters are the exact same as strptime.
    std::istringstream input(s);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(tm, f);
    if (input.fail())
    {
        return nullptr;
    }
    return (char*)(s + input.tellg());
}
time_t timegm(struct tm* tm)
{
    struct tm my_tm;

    memcpy(&my_tm, tm, sizeof(struct tm));

    /* _mkgmtime() changes the value of the struct tm* you pass in, so
     * use a copy
     */
    return _mkgmtime(&my_tm);
}
#endif

namespace utils
{

void setServerLogLevel(const std::string& level)
{
	if (level == "debug")//[debug|trace|info|warn|error|fatal]
	{
		Logger::setLogLevel(Logger::kDebug);
	}
	else if (level == "trace")
	{
		Logger::setLogLevel(Logger::kTrace);
	}
	else if (level == "info")
	{
		Logger::setLogLevel(Logger::kInfo);
	}
	else if (level == "warn")
	{
		Logger::setLogLevel(Logger::kWarn);
	}
	else if (level == "error")
	{
		Logger::setLogLevel(Logger::kError);
	}
	else if (level == "fatal")
	{
		Logger::setLogLevel(Logger::kFatal);
	}
	else
	{
		Logger::setLogLevel(Logger::kDebug);
	}
}

bool isInteger(const std::string &str)
{
    for (auto const &c : str) {
        if (c > '9' || c < '0')
            return false;
    }
    return true;
}

std::string genRandomString(int length)
{
    static const char char_space[] =
            "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::once_flag once;
    static const size_t len = strlen(char_space);
    static const int randMax = RAND_MAX - (RAND_MAX % len);
    std::call_once(once, []() {
        std::srand(static_cast<unsigned int>(time(nullptr)));
    });

    int i;
    std::string str;
    str.resize(length);

    for (i = 0; i < length; ++i) {
        int x = std::rand();
        while (x >= randMax) {
            x = std::rand();
        }
        x = (x % len);
        str[i] = char_space[x];
    }

    return str;
}

std::vector<char> hexToBinaryVector(const char *ptr, size_t length)
{
    if (length % 2 != 0)
    {
        throw CommonException("hexToBinaryVector --> length % 2 != 0");
    }
    std::vector<char> ret(length / 2, '\0');
    for (size_t i = 0; i < ret.size(); ++i) {
        auto p = i * 2;
        char c1 = ptr[p];
        if (c1 >= '0' && c1 <= '9') {
            c1 -= '0';
        } else if (c1 >= 'a' && c1 <= 'f') {
            c1 -= 'a';
            c1 += 10;
        } else if (c1 >= 'A' && c1 <= 'F') {
            c1 -= 'A';
            c1 += 10;
        } else {
            return std::vector<char>();
        }
        char c2 = ptr[p + 1];
        if (c2 >= '0' && c2 <= '9') {
            c2 -= '0';
        } else if (c2 >= 'a' && c2 <= 'f') {
            c2 -= 'a';
            c2 += 10;
        } else if (c2 >= 'A' && c2 <= 'F') {
            c2 -= 'A';
            c2 += 10;
        } else {
            return std::vector<char>();
        }
        ret[i] = c1 * 16 + c2;
    }
    return ret;
}

std::string hexToBinaryString(const char *ptr, size_t length) 
{
	if (length % 2 != 0)
	{
		throw CommonException("hexToBinaryVector --> length % 2 != 0");
	}
    std::string ret(length / 2, '\0');
    for (size_t i = 0; i < ret.length(); ++i) {
        auto p = i * 2;
        char c1 = ptr[p];
        if (c1 >= '0' && c1 <= '9') {
            c1 -= '0';
        } else if (c1 >= 'a' && c1 <= 'f') {
            c1 -= 'a';
            c1 += 10;
        } else if (c1 >= 'A' && c1 <= 'F') {
            c1 -= 'A';
            c1 += 10;
        } else {
            return "";
        }
        char c2 = ptr[p + 1];
        if (c2 >= '0' && c2 <= '9') {
            c2 -= '0';
        } else if (c2 >= 'a' && c2 <= 'f') {
            c2 -= 'a';
            c2 += 10;
        } else if (c2 >= 'A' && c2 <= 'F') {
            c2 -= 'A';
            c2 += 10;
        } else {
            return "";
        }
        ret[i] = c1 * 16 + c2;
    }
    return ret;
}

std::string binaryStringToHex(const unsigned char *ptr, size_t length)
{
    std::string idString;
    for (size_t i = 0; i < length; ++i)
    {
        int value = (ptr[i] & 0xf0) >> 4;
        if (value < 10) 
        {
            idString.append(1, char(value + 48));
        } 
        else 
        {
            idString.append(1, char(value + 55));
        }

        value = (ptr[i] & 0x0f);
        if (value < 10) 
        {
            idString.append(1, char(value + 48));
        }
        else 
        {
            idString.append(1, char(value + 55));
        }
    }
    return idString;
}

std::vector<std::string> splitString(const std::string &s,
                                     const std::string &delimiter,
                                     bool acceptEmptyString)
{
    if (delimiter.empty())
        return std::vector<std::string>{};
    std::vector<std::string> v;
    size_t last = 0;
    size_t next = 0;
    while ((next = s.find(delimiter, last)) != std::string::npos) 
    {
        if (next > last || acceptEmptyString)
            v.push_back(s.substr(last, next - last));
        last = next + delimiter.length();
    }
    if (s.length() > last || acceptEmptyString)
        v.push_back(s.substr(last));
    return v;
}

std::set<std::string> splitStringToSet(const std::string &str,
                                       const std::string &separator)
{
    std::set<std::string> ret;
    std::string::size_type pos1, pos2;
    pos2 = 0;
    pos1 = str.find(separator);
    while (pos1 != std::string::npos) 
    {
        if (pos1 != 0) 
        {
            std::string item = str.substr(pos2, pos1 - pos2);
            ret.insert(item);
        }
        pos2 = pos1 + separator.length();
        while (pos2 < str.length() &&
               str.substr(pos2, separator.length()) == separator)
            pos2 += separator.length();
        pos1 = str.find(separator, pos2);
    }
    if (pos2 < str.length())
        ret.insert(str.substr(pos2));
    return ret;
}

std::string charToHex(char c)
{
    std::string result;
    char first, second;

    first = (c & 0xF0) / 16;
    first += first > 9 ? 'A' - 10 : '0';
    second = c & 0x0F;
    second += second > 9 ? 'A' - 10 : '0';

    result.append(1, first);
    result.append(1, second);

    return result;
}

std::string formattedString(const char *format, ...)
{
    std::string strBuffer(128, 0);
    va_list ap, backup_ap;
    va_start(ap, format);
    va_copy(backup_ap, ap);
    auto result = vsnprintf((char *) strBuffer.data(),
                            strBuffer.size(),
                            format,
                            backup_ap);
    va_end(backup_ap);
    if ((result >= 0) && ((std::string::size_type) result < strBuffer.size())) 
    {
        strBuffer.resize(result);
    } 
    else 
    {
        while (true) 
        {
            if (result < 0) 
            {
                // Older snprintf() behavior. Just try doubling the buffer size
                strBuffer.resize(strBuffer.size() * 2);
            } 
            else
            {
                strBuffer.resize(result + 1);
            }

            va_copy(backup_ap, ap);
            auto result = vsnprintf((char *) strBuffer.data(),
                                    strBuffer.size(),
                                    format,
                                    backup_ap);
            va_end(backup_ap);

            if ((result >= 0) &&
                ((std::string::size_type) result < strBuffer.size())) {
                strBuffer.resize(result);
                break;
            }
        }
    }
    va_end(ap);
    return strBuffer;
}

bool bytesSize(std::string sizeStr, size_t &size)
{
    if (sizeStr.empty())
    {
        size = -1;
        return true;
    }
    else
    {
        size = 1;
        switch (sizeStr[sizeStr.length() - 1])
        {
            case 'k':
            case 'K':
                size = 1024;
                sizeStr.resize(sizeStr.length() - 1);
                break;
            case 'M':
            case 'm':
                size = (1024 * 1024);
                sizeStr.resize(sizeStr.length() - 1);
                break;
            case 'g':
            case 'G':
                size = (1024 * 1024 * 1024);
                sizeStr.resize(sizeStr.length() - 1);
                break;
#if ((ULONG_MAX) != (UINT_MAX))
                // 64bit system
        case 't':
        case 'T':
            size = (1024L * 1024L * 1024L * 1024L);
            sizeStr.resize(sizeStr.length() - 1);
            break;
#endif
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '7':
            case '8':
            case '9':
                break;
            default:
                return false;
                break;
        }
        std::istringstream iss(sizeStr);
        size_t tmpSize;
        iss >> tmpSize;
        if (iss.fail())
        {
            return false;
        }
        if ((size_t(-1) / tmpSize) >= size)
            size *= tmpSize;
        else
        {
            size = -1;
        }
        return true;
    }
}

std::string demangle(const char *mangled_name)
{
#ifndef _WIN32
    std::size_t len = 0;
    int status = 0;
    std::unique_ptr<char, decltype(&std::free)> ptr(
            __cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status),
            &std::free);
    if (status == 0)
    {
        return std::string(ptr.get());
    }
	throw CommonException("demangle --> demangle error!");
#else
    auto pos = strstr(mangled_name, " ");
    if (pos == nullptr)
        return std::string{mangled_name};
    else
        return std::string{pos + 1};
#endif
}

int createPath(const std::string& path)
{
	auto tmpPath = path;
	std::stack<std::string> pathStack;
#ifdef _WIN32
	while (_access(tmpPath.c_str(), 06) != 0)
#else
	while (access(tmpPath.c_str(), F_OK) != 0)
#endif
	{
		if (tmpPath == "./" || tmpPath == "/")
			return -1;
		while (tmpPath[tmpPath.length() - 1] == '/')
			tmpPath.resize(tmpPath.length() - 1);
		auto pos = tmpPath.rfind('/');
		if (pos != std::string::npos)
		{
			pathStack.push(tmpPath.substr(pos));
			tmpPath = tmpPath.substr(0, pos + 1);
		}
		else
		{
			pathStack.push(tmpPath);
			tmpPath.clear();
			break;
		}
	}
	while (pathStack.size() > 0)
	{
		if (tmpPath.empty())
		{
			tmpPath = pathStack.top();
		}
		else
		{
			if (tmpPath[tmpPath.length() - 1] == '/')
			{
				tmpPath.append(pathStack.top());
			}
			else
			{
				tmpPath.append("/").append(pathStack.top());
			}
		}
		pathStack.pop();

#ifdef _WIN32
		if (_mkdir(tmpPath.c_str()) == -1)
#else
		if (mkdir(tmpPath.c_str(), 0755) == -1)
#endif
		{
			LOG_ERROR << "Can't create path:" << path;
			return -1;
		}
	}
	return 0;
}


}