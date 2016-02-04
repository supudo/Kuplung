//
//  FontsList.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/4/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "FontsList.hpp"

std::vector<std::string> FontsList::getSystemFonts() {
    std::vector<std::string> systemFonts;
#ifdef defined(_WIN32)
    // ..
#elif defined(_APPLE_) && defined(_MACH_)

#else defined(linux) || defined(__linux)
    // ..
#endif
    return systemFonts;
}
