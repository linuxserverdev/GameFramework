#include "PerfTool.h"
#include <Hpp/NonCopyable.h>
#include <Logger/Logger.h>
#include <Share/Utils.h>
#include <Share/Date.h>
#include <map>

class PerformEval : public NonCopyable
{
public:
    struct EvalData
    {
        uint64_t times_{0};
        uint64_t totalTime_{0};
        uint64_t recentTimes_{0};
        uint64_t recentTotalTime_{0};
        uint64_t maxTime_{0};
        uint64_t minTime_{3600000};
    };
    using PerformEvalMap = std::map<std::string, EvalData>;
    using OutVector = std::vector<PerformEvalMap::iterator>;
public:
    static PerformEval& getIns()
    {
        static PerformEval ins;
        return ins;
    }
    uint64_t beginCall(const std::string& callName);
    void endCall(const std::string& callName, uint64_t begTicks);
    void printCall(const std::string& callName);
    void printAll();

private:
    PerformEval(){}
    PerformEvalMap perfMap_;
};

uint64_t PerformEval::beginCall(const std::string &callName)
{
#ifdef __PERF_ON__
    return Date::now().microSecondsSinceEpoch();
#endif
    return 0;
}

void PerformEval::endCall(const std::string &callName, uint64_t begTicks)
{
#ifdef __PERF_ON__
    PerformEvalMap::iterator it = perfMap_.find(callName);
    if(it != perfMap_.end())
    {
        EvalData& data = it->second;
        data.times_++;
        data.recentTimes_++;
        uint64_t callTime = Date::now().microSecondsSinceEpoch() - begTicks;
        data.totalTime_ += callTime;
        data.recentTotalTime_ += callTime;
        if(callTime > data.maxTime_) data.maxTime_ = callTime;
        if(callTime < data.minTime_) data.minTime_ = callTime;
    }
    else
    {
        uint64_t callTime = Date::now().microSecondsSinceEpoch() - begTicks;
        EvalData evalData;
        evalData.times_ = 1;
        evalData.recentTimes_ = 1;
        evalData.totalTime_ = callTime;
        evalData.recentTotalTime_ = callTime;
        evalData.maxTime_ = callTime;
        evalData.minTime_ = callTime;
        perfMap_[callName] = evalData;
    }
#endif
}

void PerformEval::printCall(const std::string &callName)
{
#ifdef __PERF_ON__
    PerformEvalMap::iterator it = perfMap_.find(callName);
    if(it != perfMap_.end())
    {
        LOG_INFO << utils::formattedString(
                    "%s perf: timesCalled=%d maxTicks=%d minTicks=%d ticksPerCall=%.3f",
                    callName.c_str(),
                    it->second.times_,
                    it->second.maxTime_,
                    it->second.minTime_,
                    it->second.times_ > 0 ? it->second.totalTime_/float(it->second.times_) : 0.0);
    }
#endif
}

void PerformEval::printAll()
{
#ifdef __PERF_ON__
    for(PerformEvalMap::iterator it = perfMap_.begin();
        it != perfMap_.end(); it++)
    {
        LOG_INFO <<utils::formattedString(
                    "%s perf: timesCalled=%d maxTicks=%d minTicks=%d ticksPerCall=%.3f recentCall=%d, recentPerCall=%.3f",
                    it->first.c_str(),
                    it->second.times_,
                    it->second.maxTime_,
                    it->second.minTime_,
                    it->second.times_ > 0 ? it->second.totalTime_/float(it->second.times_) : 0.0,
                    it->second.recentTimes_,
                    it->second.recentTimes_ > 0 ? it->second.recentTotalTime_/float(it->second.recentTimes_) : 0.0);

        it->second.recentTimes_ = 0;
        it->second.recentTotalTime_ = 0;
    }
#endif
}

PerfTool::PerfTool(std::string callName)
{
    callName_ = std::move(callName);
    begTicks_ = PerformEval::getIns().beginCall(callName_);
}

PerfTool::~PerfTool()
{
    PerformEval::getIns().endCall(callName_, begTicks_);
}

void PerfTool::printCall(const std::string &callName)
{
    PerformEval::getIns().printCall(callName);
}

void PerfTool::printAll()
{
    PerformEval::getIns().printAll();
}


