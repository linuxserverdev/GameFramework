#include "Share/TimerQueue.h"
#include "Share/Reactor.h"
#include <string.h>
#include <iostream>

int64_t howMuchTimeFromNow(const TimePoint &when)
{
    auto microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(
                            when - std::chrono::steady_clock::now())
                            .count();
    if (microSeconds < 1000)
    {
        microSeconds = 1000;
    }
    return microSeconds / 1000;
}
void TimerQueue::processTimers()
{
    reactor_->assertInLoopThread();
    const auto now = std::chrono::steady_clock::now();

    std::vector<TimerPtr> expired = getExpired(now);

    if (expired.empty()) return;

    for (auto const &timerPtr : expired)
    {
        if (timerIdSet_.find(timerPtr->id()) != timerIdSet_.end())
        {
            timerPtr->run();
        }
    }

    reset(expired, now);
}

TimerQueue::TimerQueue(Reactor* reactor)
    : reactor_(reactor),
      timers_()
{

}

TimerQueue::~TimerQueue()
{
}

TimerId TimerQueue::addTimer(const TimerCallback &cb,
                             const TimePoint &when,
                             const TimeInterval &interval)
{
    std::shared_ptr<Timer> timerPtr =
        std::make_shared<Timer>(cb, when, interval);

    reactor_->runInLoop([this, timerPtr]() { addTimerInLoop(timerPtr); });
    return timerPtr->id();
}
TimerId TimerQueue::addTimer(TimerCallback &&cb,
                             const TimePoint &when,
                             const TimeInterval &interval)
{
    std::shared_ptr<Timer> timerPtr =
        std::make_shared<Timer>(std::move(cb), when, interval);

    reactor_->runInLoop([this, timerPtr]() { addTimerInLoop(timerPtr); });
    return timerPtr->id();
}
void TimerQueue::addTimerInLoop(const TimerPtr &timer)
{
    reactor_->assertInLoopThread();
    timerIdSet_.insert(timer->id());
    if (insert(timer))
    {
    }
}

void TimerQueue::invalidateTimer(TimerId id)
{
    reactor_->runInLoop([this, id]() { timerIdSet_.erase(id); });
}

bool TimerQueue::insert(const TimerPtr &timerPtr)
{
    reactor_->assertInLoopThread();
    bool earliestChanged = false;
    if (timers_.size() == 0 || *timerPtr < *timers_.top())
    {
        earliestChanged = true;
    }
    timers_.push(timerPtr);
    // std::cout<<"after push new
    // timer:"<<timerPtr->when().microSecondsSinceEpoch()/1000000<<std::endl;
    return earliestChanged;
}

int64_t TimerQueue::getTimeout() const
{
    reactor_->assertInLoopThread();
    if (timers_.empty())
    {
        return 10000;
    }
    else
    {
        return howMuchTimeFromNow(timers_.top()->when());
    }
}

std::vector<TimerPtr> TimerQueue::getExpired(const TimePoint &now)
{
    std::vector<TimerPtr> expired;
    while (!timers_.empty())
    {
        if (timers_.top()->when() < now)
        {
            expired.push_back(timers_.top());
            timers_.pop();
        }
        else
            break;
    }
    return expired;
}

void TimerQueue::reset(const std::vector<TimerPtr>& expired,
    const TimePoint& now)
{
    reactor_->assertInLoopThread();
    for (auto const& timerPtr : expired)
    {
        if (timerPtr->isRepeat() &&
            timerIdSet_.find(timerPtr->id()) != timerIdSet_.end())
        {
            timerPtr->restart(now);
            insert(timerPtr);
        }
    }
}
