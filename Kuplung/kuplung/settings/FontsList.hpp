//
//  FontsList.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/4/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef FontsList_hpp
#define FontsList_hpp

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include "Settings.h"

class FontsList {
public:
    void init(std::function<void(std::string)> doLog);
    bool fontFileExists(std::string font);
    void getFonts();
    int getSelectedFontSize();

    std::vector<FBEntity> fonts;
    const char* fontSizes[11] = {"12", "14", "16", "18", "20", "22", "24", "26", "28", "30", "32"};

private:
    std::function<void(std::string)> doLog;
    void logMessage(std::string logMessage);
    void loadFontsOSX();
    void loadFontsWindows();
    void loadFontsNix();
};

#endif /* FontsList_hpp */
