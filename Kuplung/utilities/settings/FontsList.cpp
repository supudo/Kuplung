//
//  FontsList.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/4/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "FontsList.hpp"
#include <stdio.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "utilities/settings/Settings.h"

namespace fs = boost::filesystem;

void FontsList::init(std::function<void(std::string)> doLog) {
    this->doLog = doLog;
    this->fonts.clear();
}

bool FontsList::fontFileExists(std::string font) {
    return boost::filesystem::exists(font);
}

int FontsList::getSelectedFontSize() {
    for (int i=0; i<(int)sizeof(this->fontSizes); i++) {
        if (atof(this->fontSizes[i]) == Settings::Instance()->UIFontSize) {
            return i;
        }
    }
    return 0;
}

void FontsList::getFonts() {
#ifdef _WIN32
    return this->loadFontsWindows();
#elif __APPLE__
    return this->loadFontsOSX();
#elif linux || __linux
    return this->loadFontsNix();
#else
    return {};
#endif
}

void FontsList::loadFontsOSX() {
    //| /System/Library/Fonts - Fonts necessary for the system. Do not touch these.
    //| /Library/Fonts - Additional fonts that can be used by all users. This is generally where fonts go if they are to be used by other applications.
    //| ~/Library/Fonts - Fonts specific to each user.
    //| /Network/Library/Fonts - Fonts shared for users on a network.

    std::string fontsAdditionalPath = "/Library/Fonts/";
    fs::path currentPath(fontsAdditionalPath);

    if (fs::is_directory(currentPath)) {
        fs::directory_iterator iteratorEnd;
        int i = 1;
        for (fs::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
            try {
                std::string fileExtension = iteratorFolder->path().extension().string();
                fs::file_status fileStatus = iteratorFolder->status();
                if (fileExtension == ".ttf" && (fs::is_directory(fileStatus) || fs::is_regular_file(fileStatus))) {
                    FBEntity fontFile;
                    fontFile.isFile = true;
                    fontFile.extension = "ttf";
                    fontFile.path = fontsAdditionalPath + iteratorFolder->path().filename().string();
                    fontFile.title = iteratorFolder->path().filename().string();
                    boost::replace_all(fontFile.title, ".ttf", "");
                    this->fonts.push_back(fontFile);
                    if (fontFile.path == Settings::Instance()->UIFontFile)
                        Settings::Instance()->UIFontFileIndex = i;
                    i += 1;
                }
            }
            catch (const std::exception & ex) {
                this->logMessage(iteratorFolder->path().filename().string() + " " + ex.what());
            }
        }
    }
}

void FontsList::loadFontsWindows() {
    //| %WINDIR%\fonts
}

void FontsList::loadFontsNix() {
    //| /usr/share/fonts
    //| /usr/local/share/fonts
    //| ~/.fonts
}

void FontsList::logMessage(std::string logMessage) {
    this->doLog("[FontsList] " + logMessage);
}
