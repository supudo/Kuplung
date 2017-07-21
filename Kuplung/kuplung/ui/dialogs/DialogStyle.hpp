//
//  DialogStyle.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogStyle_hpp
#define DialogStyle_hpp

#include <regex>
#include <string>
#include <vector>
#include "kuplung/utilities/imgui/imgui.h"

class DialogStyle {
public:
    void saveDefault(ImGuiStyle& style);
    void save(std::string const& fontfile, std::string const& fontsize, ImGuiStyle& style);
    ImGuiStyle& loadCurrent();
    ImGuiStyle& load(const std::string& styleFilePath);
    ImGuiStyle& loadDefault();

private:
    void saveStyles(std::string const& fontfile, std::string const& fontsize, std::string const& styleFilePath, ImGuiStyle& style);
    std::vector<std::string> splitString(const std::string &s, const std::regex& delimiter);
    ImVec4 tov4(const std::string& opValue);
    ImVec2 tov2(const std::string& opValue);
};

#endif /* DialogStyle_hpp */
