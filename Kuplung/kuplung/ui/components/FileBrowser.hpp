//
//  FileBrowser.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef FileBrowser_hpp
#define FileBrowser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui.h"
#include <functional>
#include <map>
#include <string>
#include <boost/filesystem.hpp>

typedef enum FileBrowser_ParserType {
    FileBrowser_ParserType_Own1 = 0,
    FileBrowser_ParserType_Own2 = 1,
    FileBrowser_ParserType_Assimp = 2
} FileBrowser_ParserType;

class FileBrowser {
public:
    void init(bool log, int positionX, int positionY, int width, int height, std::function<void(FBEntity, FileBrowser_ParserType, int)> processFile);
    void setStyleBrowser(bool isStyle);
    void setImageBrowser(bool isImage);
    void draw(const char* title, bool* p_opened = NULL, int TextureType = -1);

private:
    std::map<std::string, FBEntity> getFolderContents(std::string filePath);
    std::function<void(FBEntity, FileBrowser_ParserType, int)> processFile;

    void drawFiles(int TextureType = -1);
    std::string convertToString(double num);
    std::string convertSize(size_t size);
    double roundOff(double n);
    void logMessage(std::string logMessage);
    bool isHidden(const boost::filesystem::path &p);

    bool log, isStyleBrowser, isImageBrowser;
    int positionX, positionY, width, height;
};

#endif /* FileBrowser_hpp */
