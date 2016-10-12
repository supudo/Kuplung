//
//  Consumption.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Consumption.hpp"

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>

void Consumption::init() {
}

unsigned long Consumption::getMemoryConsumption() {
    struct rusage kuplung_resource_usage;
    unsigned long memory_bytes;
    if (0 == getrusage(RUSAGE_SELF, &kuplung_resource_usage)) {
        memory_bytes = kuplung_resource_usage.ru_maxrss;
        return memory_bytes / (1024 * 1024); // KB
    }
    return 0;
}

unsigned long Consumption::getCPUUsage() {
    struct rusage kuplung_resource_usage;
    if (0 == getrusage(RUSAGE_SELF, &kuplung_resource_usage)) {
        printf("Total User CPU = %ld.%ld\n",
                kuplung_resource_usage.ru_utime.tv_sec,
                kuplung_resource_usage.ru_utime.tv_usec);
        printf("Total System CPU = %ld.%ld\n",
                kuplung_resource_usage.ru_stime.tv_sec,
                kuplung_resource_usage.ru_stime.tv_usec);
    }
}
