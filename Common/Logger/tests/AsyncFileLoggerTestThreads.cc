#include <logger/Logger.h>
#include <logger/AsyncFileLogger.h>
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

    int ThreadsNum = 10;
    uint64_t logMsgCount = 1000000;
    std::vector<std::thread> logThreads;

    for (int i=0; i < ThreadsNum ; i++)
    {
        logThreads.emplace_back([logMsgCount]{
            uint64_t count{0};
            while (count < logMsgCount)
            {
                ++count;
                if (count % 100 == 0)
                {
                    LOG_ERROR << "this is the " << count << "th log";
                    continue;
                }
                LOG_INFO << "this is the " << count << "th log";
                ++count;
                LOG_DEBUG << "this is the " << count << "th log";
                //std::this_thread::sleep_for(1s);
            }
        });
    }


    for(auto& t : logThreads)
    {
        if(t.joinable())
        {
            t.join();
        }
    }

}
