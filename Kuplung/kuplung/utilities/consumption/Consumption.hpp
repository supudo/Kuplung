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

class Consumption {
public:
    void init();
    unsigned long getMemoryConsumption();
    unsigned long getCPUUsage();
};

#endif /* Consumption_hpp */
