#pragma once

#include "Hpp/NonCopyable.h"
#include "Timer.h"
#include <queue>
#include <memory>
#include <atomic>
#include <unordered_set>

// class Timer;
class Reactor;
using TimerPtr = std::shared_ptr<Timer>;
struct comp
{
    bool operator()(const TimerPtr &x, const TimerPtr &y) const
    {
        return *x > *y;
    }
};

class TimerQueue : NonCopyable
{
  public:
    explicit TimerQueue(Reactor* reactor);
    ~TimerQueue();
    TimerId addTimer(const TimerCallback &cb,
                     const TimePoint &when,
                     const TimeInterval &interval);
    TimerId addTimer(TimerCallback &&cb,
                     const TimePoint &when,
                     const TimeInterval &interval);
    void addTimerInLoop(const TimerPtr &timer);
    void invalidateTimer(TimerId id);
    int64_t getTimeout() const;
    void processTimers();
 protected:
    Reactor* reactor_;
    std::priority_queue<TimerPtr, std::vector<TimerPtr>, comp> timers_;

    void reset(const std::vector<TimerPtr> &expired, const TimePoint &now);
    std::vector<TimerPtr> getExpired(const TimePoint &now);

  private:
    std::unordered_set<uint64_t> timerIdSet_;
};
