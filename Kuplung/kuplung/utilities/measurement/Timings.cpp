//
//  Timings.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Timings.hpp"

namespace KuplungApp { namespace Utilities { namespace Measurement {

std::chrono::high_resolution_clock::time_point tStart;
std::chrono::high_resolution_clock::time_point tEnd;

void timingStart() {
    tStart = std::chrono::high_resolution_clock::now();
}

void timingEnd() {
    tEnd = std::chrono::high_resolution_clock::now();
}

void timingPrint() {
    auto durationMS = std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count();
    auto duration_seconds = durationMS * pow(10, -6);
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[TIMINGS] %f seconds", duration_seconds));
}

void timingPrintPretty(const char* fileName, const char* functionName, const int lineNumber) {
    auto durationMS = std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tStart).count();
    auto durationS = durationMS * pow(10, -6);
    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[TIMINGS] %f s: [%s] @ [%s] on line [%i]", durationS, fileName, functionName, lineNumber));
}

}}}
