//
//  WindowsCPUUsage.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef WindowsCPUUsage_hpp
#define WindowsCPUUsage_hpp

namespace KuplungApp::Utilities::Consumption {

#include <Windows.h>

class WindowsCPUUsage {
public:
  WindowsCPUUsage();
  short GetUsage();

private:
  ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
  bool EnoughTimePassed();
  inline bool IsFirstRun() const { return (m_dwLastRun == 0); }

  //system total times
  FILETIME m_ftPrevSysKernel;
  FILETIME m_ftPrevSysUser;

  //process times
  FILETIME m_ftPrevProcKernel;
  FILETIME m_ftPrevProcUser;

  short m_nCpuUsage;
  ULONGLONG m_dwLastRun;

  volatile LONG m_lRunCount;
};

}

#endif /* WindowsCPUUsage_hpp */
