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
    this->usageOverall = (this->usageMemory == "" ? "Memory: n/a" : this->usageMemory) + ", " + (this->usageCPU == "" ? "CPU: n/a" : this->usageCPU);
    return this->usageOverall;
}

std::string Consumption::getMemoryConsumption() {
    if (this->isTimeToUpdateMemory()) {
        double memory = this->getPeakRSS() - this->memoryMarkPoint;
        if (std::fabs(this->memoryMarkPoint) < 1e-6)
            this->usageMemory = Settings::Instance()->string_format("Total Memory: %.2fMb", memory);
        else
            this->usageMemory = Settings::Instance()->string_format("Memory: %.2fMb", memory);
    }
    return this->usageMemory;
}

std::string Consumption::getCPULoad() {
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
    if (Settings::Instance()->Consumption_Interval_CPU > 0 && (this->currentTimeCPU > this->lastTimeCPU + (1000 * Settings::Instance()->Consumption_Interval_CPU))) {
        this->lastTimeCPU = this->currentTimeCPU;
        return true;
    }
    return false;
}

std::string Consumption::exec(const char* cmd) {
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

}}}
