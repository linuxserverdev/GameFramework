#include <share/PerfTool.h>
#include <logger/Logger.h>
#include <logger/AsyncFileLogger.h>
#include <iostream>

using namespace std::chrono_literals;
int main()
{
    AsyncFileLogger& asyncFileLogger = AsyncFileLogger::getIns();
    asyncFileLogger.setFileName("async_test");
    asyncFileLogger.startLogging();
    Logger::setOutputFunction(
            [&](const char *msg, const uint64_t len) {
                asyncFileLogger.output(msg, len);
            },
            [&]() { asyncFileLogger.flush(); });
    asyncFileLogger.setFileSizeLimit(100000000);
    {
        PerfTool ("logger_test");
        int i = 0;
        while (i < 1000000)
        {
            ++i;
            if (i % 100 == 0)
            {
                LOG_ERROR << "this is the " << i << "th log";
                continue;
            }
            LOG_INFO << "this is the " << i << "th log";
            ++i;
            LOG_DEBUG << "this is the " << i << "th log";
        }
    }
    PerfTool::printAll();
}
