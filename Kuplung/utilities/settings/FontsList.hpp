//
//  FontsList.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/4/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef FontsList_hpp
#define FontsList_hpp

#include <functional>
#include <string>
#include <vector>
#include "Settings.h"

class FontsList {
public:
    void init(std::function<void(std::string)> doLog);
    bool fontFileExists(std::string font);
    void getFonts();

    std::vector<FBEntity> fonts;

private:
    std::function<void(std::string)> doLog;
    void logMessage(std::string logMessage);
    void loadFontsOSX();
    void loadFontsWindows();
    void loadFontsNix();
};

#endif /* FontsList_hpp */
