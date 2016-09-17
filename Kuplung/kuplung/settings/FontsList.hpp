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
#include <string>
#include <vector>
#include "Settings.h"

class FontsList {
public:
    void init();
    bool fontFileExists(std::string const& font);
    void getFonts();
    int getSelectedFontSize();

    std::vector<FBEntity> fonts;
    const char* fontSizes[11] = {"12", "14", "16", "18", "20", "22", "24", "26", "28", "30", "32"};

private:
    void loadFontsOSX();
    void loadFontsWindows();
    void loadFontsNix();
};

#endif /* FontsList_hpp */
