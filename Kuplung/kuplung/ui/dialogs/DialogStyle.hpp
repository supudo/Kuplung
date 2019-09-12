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
  void saveDefault(ImGuiStyle& style) const;
  void save(std::string const& fontfile, std::string const& fontsize, const ImGuiStyle& style) const;
  const ImGuiStyle& loadCurrent() const;
  const ImGuiStyle& load(const std::string& styleFilePath) const;
  const ImGuiStyle& loadDefault() const;

private:
  void saveStyles(std::string const& fontfile, std::string const& fontsize, std::string const& styleFilePath, const ImGuiStyle& style) const;
  const std::vector<std::string> splitString(const std::string &s, const std::regex& delimiter) const;
  const ImVec4 tov4(const std::string& opValue) const;
  const ImVec2 tov2(const std::string& opValue) const;
};

#endif /* DialogStyle_hpp */
