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
#include "ImGui/imgui.h"

class GUIStyle {
public:
    void init(std::function<void(std::string)> doLog);
    void saveDefault(ImGuiStyle& style);
    void save(ImGuiStyle& style);
    void load(std::string styleFilePath);
    void loadDefault();

private:
    std::function<void(std::string)> doLog;

    void saveStyles(std::string styleFilePath, ImGuiStyle& style);
    std::vector<std::string> splitString(const std::string &s, std::regex delimiter);
};

#endif /* GUIStyle_hpp */
