//
//  Timings.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Timings_hpp
#define Timings_hpp

#include <chrono>
#include "kuplung/settings/Settings.h"

#define TimingStart() KuplungApp::Utilities::Measurement::timingStart();
#define TimingEnd() KuplungApp::Utilities::Measurement::timingEnd();
#define TimingPrint() KuplungApp::Utilities::Measurement::timingPrint();
#define TimingPrintPretty(fileName, functionName, lineNumber) KuplungApp::Utilities::Measurement::timingPrintPretty(fileName, functionName, lineNumber);

namespace KuplungApp { namespace Utilities { namespace Measurement {

void timingStart();
void timingEnd();
void timingPrint();
// KuplungApp::Utilities::Measurement::timingPrintPretty(__FILE__, __PRETTY_FUNCTION__, __LINE__);
void timingPrintPretty(const char* fileName, const char* functionName, const int lineNumber);

}}}

#endif /* Timings_hpp */
