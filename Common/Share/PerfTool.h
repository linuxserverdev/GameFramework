#pragma once
#include <string>
#define __PERF_ON__

struct PerfTool
{
    std::string callName_;
    uint64_t begTicks_;
    PerfTool(std::string callName);
    ~PerfTool();
    static void printCall(const std::string &callName);
    static void printAll();
};

