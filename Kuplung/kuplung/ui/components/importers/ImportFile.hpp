//
//  ImportFile.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ImportFile_hpp
#define ImportFile_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include <functional>
#include <map>
#include <string>
#include <boost/filesystem.hpp>

class ImportFile {
public:
    void init(int positionX, int positionY, int width, int height,
              const std::function<void(FBEntity, std::vector<std::string>)>& processFile);
    void draw(const char* title, bool* p_opened = NULL, int type=0);

private:
    std::map<std::string, FBEntity> getFolderContents(std::string const& filePath, int type);
    std::function<void(FBEntity, std::vector<std::string>)> processFile;

    void drawFiles(const std::string& fPath, int type);
    std::string convertToString(double num);
    std::string convertSize(size_t size);
    double roundOff(double n);
    bool isHidden(const boost::filesystem::path &p);

    int positionX, positionY, width, height;
    float panelWidth_Options, panelWidth_OptionsMin;

    int Setting_Forward, Setting_Up;

	std::string currentFolder;
};

#endif /* ImportFile_hpp */
