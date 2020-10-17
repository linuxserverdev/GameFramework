#pragma once

#include <stdint.h>
#include <string>

#define MICRO_SECONDS_PRE_SEC 1000000

class Date
{
  public:
    Date() : microSecondsSinceEpoch_(0){};
    explicit Date(int64_t microSec) : microSecondsSinceEpoch_(microSec){};
    Date(unsigned int year,
         unsigned int month,
         unsigned int day,
         unsigned int hour = 0,
         unsigned int minute = 0,
         unsigned int second = 0,
         unsigned int microSecond = 0);
    ~Date() {};

    static const Date date();

    static const Date now()
    {
        return Date::date();
    }

    const Date after(double second) const;
    const Date roundSecond() const;
    const Date roundDay() const;
    

    bool operator==(const Date &date) const
    {
        return microSecondsSinceEpoch_ == date.microSecondsSinceEpoch_;
    }
    bool operator!=(const Date &date) const
    {
        return microSecondsSinceEpoch_ != date.microSecondsSinceEpoch_;
    }
    bool operator<(const Date &date) const
    {
        return microSecondsSinceEpoch_ < date.microSecondsSinceEpoch_;
    }
    bool operator>(const Date &date) const
    {
        return microSecondsSinceEpoch_ > date.microSecondsSinceEpoch_;
    }
    bool operator>=(const Date &date) const
    {
        return microSecondsSinceEpoch_ >= date.microSecondsSinceEpoch_;
    }
    bool operator<=(const Date &date) const
    {
        return microSecondsSinceEpoch_ <= date.microSecondsSinceEpoch_;
    }
    int64_t microSecondsSinceEpoch() const
    {
        return microSecondsSinceEpoch_;
    }
    struct tm tmStruct() const;

    /**
     * Example:
     * 20180101 10:10:25            //If the @param showMicroseconds is false
     * 20180101 10:10:25:102414     //If the @param showMicroseconds is true
     */
    std::string toFormattedStringLocal(bool showMicroseconds) const;

    /**
     * The @param fmtStr is the format string for the function strftime()
     * Example:
     * 2018-01-01 10:10:25          //If the @param fmtStr is "%Y-%m-%d %H:%M:%S" and the @param showMicroseconds is false 
     * 2018-01-01 10:10:25:102414   //If the @param fmtStr is "%Y-%m-%d %H:%M:%S" and the
     * @param showMicroseconds is true
     */
    std::string toCustomedFormattedStringLocal(
        const std::string &fmtStr,
        bool showMicroseconds = false) const;

    bool isSameSecond(const Date &date) const
    {
        return microSecondsSinceEpoch_ / MICRO_SECONDS_PRE_SEC ==
               date.microSecondsSinceEpoch_ / MICRO_SECONDS_PRE_SEC;
    }

    void swap(Date &that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

  private:
    int64_t microSecondsSinceEpoch_{0};
};
