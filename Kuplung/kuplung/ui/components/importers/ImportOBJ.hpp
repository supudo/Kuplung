//
//  ImportOBJ.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ImportOBJ_hpp
#define ImportOBJ_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include <functional>
#include <map>
#include <string>
#include <boost/filesystem.hpp>

class ImportOBJ {
public:
    void init(int positionX, int positionY, int width, int height,
              std::function<void(FBEntity, std::vector<std::string>)> processFile);
    void draw(const char* title, bool* p_opened = NULL);

private:
    std::map<std::string, FBEntity> getFolderContents(std::string const& filePath);
    std::function<void(FBEntity, std::vector<std::string>)> processFile;

    void drawFiles();
    std::string convertToString(double num);
    std::string convertSize(size_t size);
    double roundOff(double n);
    bool isHidden(const boost::filesystem::path &p);

    int positionX, positionY, width, height;
    float panelWidth_Options, panelWidth_OptionsMin;

    int Setting_Forward, Setting_Up;
};

#endif /* ImportOBJ_Type_hpp */
