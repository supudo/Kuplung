//
//  Consumption.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Consumption.hpp"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <mach/mach.h>
#endif
#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"

namespace KuplungApp { namespace Utilities { namespace Consumption {

void Consumption::init() {
    this->usageOverall = "";
    this->usageMemory = "";
    this->usageCPU = "";
    this->memoryMark();
}

std::string Consumption::getOverallStats() {
    this->usageCPU = this->getCPULoad();
    this->usageMemory = this->getMemoryConsumption();
    this->usageOverall = (this->usageMemory.empty() ? "Memory: n/a" : this->usageMemory) + ", " + (this->usageCPU.empty() ? "CPU: n/a" : this->usageCPU);
    return this->usageOverall;
}

std::string Consumption::getMemoryConsumption() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.WorkingSetSize;
    return Settings::Instance()->string_format("Memory: %.2fMb", virtualMemUsedByMe);
#else
    if (this->isTimeToUpdateMemory()) {
        double memory = this->getPeakRSS() - this->memoryMarkPoint;
        if (std::fabs(this->memoryMarkPoint) < 1e-6)
            this->usageMemory = Settings::Instance()->string_format("Total Memory: %.2fMb", memory);
        else
            this->usageMemory = Settings::Instance()->string_format("Memory: %.2fMb", memory);
    }
    return this->usageMemory;
#endif
}

#ifdef _WIN32
static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks) {
   static unsigned long long _previousTotalTicks = 0;
   static unsigned long long _previousIdleTicks = 0;

   unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
   unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

   float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

   _previousTotalTicks = totalTicks;
   _previousIdleTicks = idleTicks;
   return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME& ft) {
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}
#endif

std::string Consumption::getCPULoad() {
#ifdef _WIN32
    if (this->isTimeToUpdateCPU()) {
        FILETIME idleTime, kernelTime, userTime;
        float cpu = GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
        cpu *= 100.0f;
        this->usageCPU = "CPU: " + Settings::Instance()->string_format("%.2f", cpu) + "%";
    }
    return this->usageCPU;
#else
    if (this->isTimeToUpdateCPU()) {
        pid_t p = getpid();
        std::string c = Settings::Instance()->string_format("ps -p %i -o pcpu", int(p));
        const char* com = c.c_str();
        std::string res = this->exec(com);

        std::vector<std::string> elems;
        boost::split(elems, res, boost::is_any_of("\n"));

        this->usageCPU = "CPU:" + elems[1] + "%";
    }
    return this->usageCPU;
#endif
}

bool Consumption::isTimeToUpdateMemory() {
    this->currentTimeMemory = SDL_GetTicks();
    if (Settings::Instance()->Consumption_Interval_Memory > 0 && (this->currentTimeMemory > this->lastTimeMemory + (1000 * Settings::Instance()->Consumption_Interval_Memory))) {
        this->lastTimeMemory = this->currentTimeMemory;
        return true;
    }
    return false;
}

bool Consumption::isTimeToUpdateCPU() {
    this->currentTimeCPU = SDL_GetTicks();
#ifdef _WIN32
    if (Settings::Instance()->Consumption_Interval_CPU > 0 && (this->currentTimeCPU > this->lastTimeCPU + (250 * Settings::Instance()->Consumption_Interval_CPU))) {
        this->lastTimeCPU = this->currentTimeCPU;
        return true;
    }
#else
    if (Settings::Instance()->Consumption_Interval_CPU > 0 && (this->currentTimeCPU > this->lastTimeCPU + (1000 * Settings::Instance()->Consumption_Interval_CPU))) {
        this->lastTimeCPU = this->currentTimeCPU;
        return true;
    }
#endif
    return false;
}

std::string Consumption::exec(const char* cmd) {
#ifdef _WIN32
    return "";
#else
    FILE* pipe = popen(cmd, "r");
    if (!pipe)
        return "ERROR";

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
#endif
}

void Consumption::memoryMark() {
    this->memoryMarkPoint = this->getPeakRSS();
}

void Consumption::memoryUnmark() {
    this->memoryMarkPoint = 0.0;
}

size_t Consumption::getPeakRSS() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.PeakWorkingSetSize;
#else
    struct rusage kuplung_resource_usage;
    getrusage(RUSAGE_SELF, &kuplung_resource_usage);
    return size_t(kuplung_resource_usage.ru_maxrss) / (1024.0 * 1024.0);
#endif
}

size_t Consumption::getCurrentRSS() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.WorkingSetSize;
#else
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS)
        return (size_t)0L;
    return (size_t)info.resident_size;
#endif
}

}}}
