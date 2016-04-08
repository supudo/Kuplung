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

typedef enum FileBrowser_ParserType {
    FileBrowser_ParserType_Own = 0,
    FileBrowser_ParserType_Assimp
} FileBrowser_ParserType;

class FileBrowser {
public:
    void init(bool log, int positionX, int positionY, int width, int height, std::function<void(FBEntity, FileBrowser_ParserType)> processFile);
    void setStyleBrowser(bool isStyle);
    void setImageBrowser(bool isImage);
    void draw(const char* title, bool* p_opened = NULL);

private:
    std::map<std::string, FBEntity> getFolderContents(std::string filePath);
    std::function<void(FBEntity, FileBrowser_ParserType)> processFile;

    void drawFiles();
    std::string convertToString(double num);
    std::string convertSize(size_t size);
    double roundOff(double n);
    void logMessage(std::string logMessage);

    bool log, isStyleBrowser, isImageBrowser;
    int positionX, positionY, width, height;
};

#endif /* FileBrowser_hpp */
