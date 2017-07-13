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

    void memoryMark();
    void memoryUnmark();

    std::string exec(const char* cmd);
};

}}}

#endif /* Consumption_hpp */
