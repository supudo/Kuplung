//
//  FileBrowser.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef FileBrowser_hpp
#define FileBrowser_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include <functional>
#include <map>
#include <string>
#include "kuplung/settings/Settings.h"

class FileBrowser {
public:
    void init(bool log, int positionX, int positionY, int width, int height, std::function<void(std::string)> doLog, std::function<void(FBEntity)> processFile);
    void setStyleBrowser(bool isStyle);
    void draw(const char* title, bool* p_opened = NULL);

private:
    std::map<std::string, FBEntity> getFolderContents(std::string filePath);
    std::function<void(std::string)> doLog;
    std::function<void(FBEntity)> processFile;

    void drawFiles();
    void logMessage(std::string logMessage);
    std::string convertToString(double num);
    std::string convertSize(size_t size);
    double roundOff(double n);

    bool log, isStyleBrowser;
    int positionX, positionY, width, height;
};

#endif /* FileBrowser_hpp */