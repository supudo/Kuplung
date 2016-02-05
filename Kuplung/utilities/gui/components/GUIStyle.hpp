//
//  GUIStyle.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef GUIStyle_hpp
#define GUIStyle_hpp

#include <regex>
#include <string>
#include <vector>
#include "utilities/gui/ImGui/imgui.h"

class GUIStyle {
public:
    void init(std::function<void(std::string)> doLog);
    void saveDefault(ImGuiStyle& style);
    void save(std::string fontfile, ImGuiStyle& style);
    ImGuiStyle& loadCurrent();
    ImGuiStyle& load(std::string styleFilePath);
    ImGuiStyle& loadDefault();

private:
    std::function<void(std::string)> doLog;

    void saveStyles(std::string fontfile, std::string styleFilePath, ImGuiStyle& style);
    std::vector<std::string> splitString(const std::string &s, std::regex delimiter);
    ImVec4 tov4(std::string opValue);
    ImVec2 tov2(std::string opValue);
};

#endif /* GUIStyle_hpp */
