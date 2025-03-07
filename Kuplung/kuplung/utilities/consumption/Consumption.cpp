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
#elif __APPLE__
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <mach/mach.h>
#endif
#include <iostream>
#include <fstream>
#include <cmath>
#include "kuplung/utilities/helpers/Strings.h"
#include "kuplung/utilities/gl/GLIncludes.h"

namespace KuplungApp { namespace Utilities { namespace Consumption {

void Consumption::init() {
  this->usageOverall.clear();
  this->usageMemory.clear();
  this->usageCPU.clear();
  this->memoryMark();
}

std::string Consumption::getOverallStats() {
  this->usageCPU = this->getCPULoad();
  this->usageMemory = this->getMemoryConsumption();
  this->usageOverall = (this->usageMemory.empty() ? "Memory: n/a" : this->usageMemory) + ", " + (this->usageCPU.empty() ? "CPU: n/a" : this->usageCPU);
  return this->usageOverall;
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

// -------------------------
// Memory
// -------------------------

std::string Consumption::getMemoryConsumption() {
#ifdef _WIN32
  if (this->isTimeToUpdateMemory()) {
    //double memory = this->getWorkingRSS() - this->memoryMarkPoint;
    double memory = this->getPagefileUsage();
    this->usageMemory = Settings::Instance()->string_format("Memory: %.2fMb", memory);
  }
  return this->usageMemory;
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

const std::string Consumption::exec(const char* cmd) const {
#ifdef _WIN32
  return "";
#else
  FILE* pipe = popen(cmd, "r");
  if (!pipe)
    return "ERROR";

  char buffer[128];
  std::string result("");
  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }
  pclose(pipe);
  return result;
#endif
}

void Consumption::memoryMark() {
#ifdef _WIN32
  this->memoryMarkPoint = this->getWorkingRSS();
#else
// this->memoryMarkPoint = this->getPeakRSS();
#endif
}

void Consumption::memoryUnmark() {
  this->memoryMarkPoint = 0.0;
}

void Consumption::windows_printMemStruct() {
#ifdef _WIN32
  PROCESS_MEMORY_COUNTERS pmc2;
  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc2, sizeof(pmc2))) {
    printf("----------------------------------------------------------------\n");
    printf("PageFaultCount: %f\n", (double)pmc2.PageFaultCount / (1024 * 1024));
    printf("PeakWorkingSetSize: %f\n", (double)pmc2.PeakWorkingSetSize / (1024 * 1024));
    printf("WorkingSetSize: %f\n", (double)pmc2.WorkingSetSize / (1024 * 1024));
    printf("QuotaPeakPagedPoolUsage: %f\n", (double)pmc2.QuotaPeakPagedPoolUsage / (1024 * 1024));
    printf("QuotaPagedPoolUsage: %f\n", (double)pmc2.QuotaPagedPoolUsage / (1024 * 1024));
    printf("QuotaPeakNonPagedPoolUsage: %f\n", (double)pmc2.QuotaPeakNonPagedPoolUsage / (1024 * 1024));
    printf("QuotaNonPagedPoolUsage: %f\n", (double)pmc2.QuotaNonPagedPoolUsage / (1024 * 1024));
    printf("PagefileUsage: %f\n", (double)pmc2.PagefileUsage / (1024 * 1024));
    printf("PeakPagefileUsage: %f\n", (double)pmc2.PeakPagefileUsage / (1024 * 1024));
  }
#endif
}

size_t Consumption::getPagefileUsage() {
#ifdef _WIN32
  //windows_printMemStruct();
  PROCESS_MEMORY_COUNTERS pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
  SIZE_T memoryUsage = pmc.PagefileUsage;
  return size_t(memoryUsage) / (1024.0 * 1024.0);
#else
  return 0;
#endif
}

size_t Consumption::getWorkingRSS() {
#ifdef _WIN32
  PROCESS_MEMORY_COUNTERS pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
  SIZE_T virtualMemUsedByMe = pmc.WorkingSetSize;
  return size_t(virtualMemUsedByMe) / (1024.0 * 1024.0);
#else
  return 0;
#endif
}

size_t Consumption::getPeakRSS() {
#ifdef _WIN32
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
  return (size_t)info.PeakWorkingSetSize;
#elif __APPLE__
  struct rusage kuplung_resource_usage;
  getrusage(RUSAGE_SELF, &kuplung_resource_usage);
  return size_t(kuplung_resource_usage.ru_maxrss) / (1024.0 * 1024.0);
#else
  return 0;
#endif
}

size_t Consumption::getCurrentRSS() {
#ifdef _WIN32
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
  return (size_t)info.WorkingSetSize;
#elif __APPLE__
  struct mach_task_basic_info info;
  mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
  if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS)
    return (size_t)0L;
  return (size_t)info.resident_size;
#else
  return 0;
#endif
}

// -------------------------
// CPU
// -------------------------

std::string Consumption::getCPULoad() {
#ifdef _WIN32
  if (this->isTimeToUpdateCPU()) {
    short cpuUsage = this->winCPUMeter.GetUsage();
    this->usageCPU = Settings::Instance()->string_format("CPU: %d%%", cpuUsage);
  }
  return this->usageCPU;
#else
  if (this->isTimeToUpdateCPU()) {
    pid_t p = getpid();
    std::string c = Settings::Instance()->string_format("ps -p %i -o pcpu", int(p));
    const char* com = c.c_str();
    std::string res = this->exec(com);
    std::vector<std::string> elems = Kuplung::Helpers::splitString(res, '\n');
    this->usageCPU = "CPU:" + elems[1] + "%";
  }
  return this->usageCPU;
#endif
}

}}}
