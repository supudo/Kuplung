//
//  Consumption.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Consumption_hpp
#define Consumption_hpp

#include "kuplung/settings/Settings.h"
#ifdef _WIN32
#include "WindowsCPUUsage.hpp"
#endif

#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN
//#define PSAPI_VERSION 2
#include <Windows.h>
#include <psapi.h>
#elif __APPLE__
#include <mach/mach.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <iostream>
#include <fstream>
#include <cmath>
#include "kuplung/utilities/helpers/Strings.h"
#include "kuplung/utilities/gl/GLIncludes.h"

namespace KuplungApp { namespace Utilities { namespace Consumption {

class Consumption {
public:
  void init();
  std::string getOverallStats();
  std::string getMemoryConsumption();
  std::string getCPULoad();

private:
  double memoryMarkPoint;
  std::string usageOverall;
  std::string usageMemory;
  std::string usageCPU;
  unsigned int lastTimeMemory = 0, currentTimeMemory;
  unsigned int lastTimeCPU = 0, currentTimeCPU;

  bool isTimeToUpdateMemory();
  bool isTimeToUpdateCPU();

  size_t getPeakRSS();
  size_t getCurrentRSS();
  size_t getWorkingRSS();
  size_t getPagefileUsage();
  void windows_printMemStruct();

  void memoryMark();
  void memoryUnmark();

  const std::string exec(const char* cmd) const;

#ifdef _WIN32
  WindowsCPUUsage winCPUMeter;
#endif
};

}}}

#endif /* Consumption_hpp */
