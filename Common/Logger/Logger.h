#pragma once
#include "LogStream.h"
#include <Hpp/NonCopyable.h>
#include <Share/Date.h>
#include <string.h>
#include <functional>
#include <iostream>

struct LoggerInfo
{
    std::string logPath;
    std::string logBaseName;
    std::string logExtName;
    std::string logLevel;
    std::string fileSize;
};

class Logger : public NonCopyable
{
  public:
    enum LogLevel
    {
        kDebug = 0,
        kTrace,
        kInfo,
        kWarn,
        kError,
        kFatal,
        kNumberOfLogLevels
    };
    // compile time calculation of basename of source file
    class SourceFile
    {
      public:
        template <int N>
        inline SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1)
        {
            // std::cout<<data_<<std::endl;
            const char *slash = strrchr(data_, '/');  // builtin function
            if (slash)
            {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char *filename) : data_(filename)
        {
            const char *slash = strrchr(filename, '/');
            if (slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char *data_;
        int size_;
    };
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, bool isSysErr);
    Logger(SourceFile file, int line, LogLevel level, const char *func);
    ~Logger();
    LogStream &stream();
    static void setOutputFunction(
        std::function<void(const char *msg, const uint64_t len)> outputFunc,
        std::function<void()> flushFunc)
    {
        outputFunc_() = outputFunc;
        flushFunc_() = flushFunc;
    }

    static void setLogLevel(LogLevel level)
    {
        logLevel_() = level;
    }
    static LogLevel logLevel()
    {
        return logLevel_();
    }

  protected:
    static void defaultOutputFunction(const char *msg, const uint64_t len)
    {
        fwrite(msg, 1, len, stdout);
    }
    static void defaultFlushFunction()
    {
        fflush(stdout);
    }
    void formatTime();
    static LogLevel &logLevel_()
    {
#ifdef NDEBUG
        static LogLevel logLevel_ = LogLevel::kInfo;
#else
        static LogLevel logLevel = LogLevel::kDebug;
#endif
        return logLevel;
    }
    static std::function<void(const char *msg, const uint64_t len)>
        &outputFunc_()
    {
        static std::function<void(const char *msg, const uint64_t len)>
            outputFunc = Logger::defaultOutputFunction;
        return outputFunc;
    }
    static std::function<void()> &flushFunc_()
    {
        static std::function<void()> flushFunc = Logger::defaultFlushFunction;
        return flushFunc;
    }
    LogStream logStream_;
    Date date_{Date::now()};
    SourceFile sourceFile_;
    int fileLine_;
    LogLevel level_;
};

extern const char *strerror_tl(int savedErrno);

#define LOG_TRACE                                        \
    if (Logger::logLevel() <= Logger::kTrace)            \
    Logger(__FILE__, __LINE__, Logger::kTrace, __func__) \
        .stream()

#define LOG_DEBUG                                        \
    if (Logger::logLevel() <= Logger::kDebug)            \
    Logger(__FILE__, __LINE__, Logger::kDebug, __func__) \
        .stream()

#define LOG_INFO                             \
    if (Logger::logLevel() <= Logger::kInfo) \
    Logger(__FILE__, __LINE__).stream()

#define LOG_WARN \
    Logger(__FILE__, __LINE__, Logger::kWarn).stream()

#define LOG_ERROR \
    Logger(__FILE__, __LINE__, Logger::kError).stream()

#define LOG_FATAL \
    Logger(__FILE__, __LINE__, Logger::kFatal).stream()

#define LOG_SYSERR \
    Logger(__FILE__, __LINE__, true).stream()


#ifndef NDEBUG
#define LOG_MASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            Logger(__FILE__, __LINE__, Logger::kFatal).stream() \
                    << "log assert `" #condition "` : " << message ; \
            exit(1); \
        } \
    } while (false)

#define LOG_CASSERT(condition) \
    do { \
        if (!(condition)) { \
            Logger(__FILE__, __LINE__, Logger::kFatal).stream() \
                      << "log assert `" #condition "` ";\
            exit(1); \
        } \
    } while (false)

#else
#define LOG_MASSERT(condition, message)
#define LOG_CASSERT(condition)
#endif

