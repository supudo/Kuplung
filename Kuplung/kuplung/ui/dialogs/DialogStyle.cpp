//
//  DialogStyle.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "DialogStyle.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include "kuplung/settings/Settings.h"

void DialogStyle::saveDefault(ImGuiStyle& style) {
    std::string defaultStyleFile = Settings::Instance()->appFolder() + "/KuplungStyleDefault.style";
    if (!boost::filesystem::exists(defaultStyleFile))
        this->saveStyles("-", "14.00", defaultStyleFile, style);
}

void DialogStyle::save(std::string fontfile, std::string fontsize, ImGuiStyle& style) {
    this->saveStyles(fontfile, fontsize, Settings::Instance()->appFolder() + "/KuplungStyle.style", style);
}

ImGuiStyle& DialogStyle::load(std::string styleFilePath) {
   ImGuiStyle& style = ImGui::GetStyle();

    std::FILE *fp = std::fopen(styleFilePath.c_str(), "rb");
    if (fp) {
        std::string fileContents;
        std::fseek(fp, 0, SEEK_END);
        fileContents.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&fileContents[0], 1, fileContents.size(), fp);
        std::fclose(fp);

        size_t pos = 0;
        std::string singleLine;
        std::regex regex_comment("^#.*");
        std::regex regex_equalsSign("=");

        while ((pos = fileContents.find(Settings::Instance()->newLineDelimiter)) != std::string::npos) {
            singleLine = fileContents.substr(0, pos);

            if (singleLine == "" || std::regex_match(singleLine, regex_comment)) {
                fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
                continue;
            }
            else {
                std::string opKey, opValue;
                std::vector<std::string> lineElements = this->splitString(singleLine, regex_equalsSign);
                opKey = lineElements[0];
                boost::algorithm::trim(opKey);

                if (lineElements.size() > 1) {
                    opValue = lineElements[1];
                    boost::algorithm::trim(opValue);
                }
                else
                    opValue = "";

                boost::replace_all(opKey, "style.rendering.", "");
                boost::replace_all(opKey, "style.sizes.", "");
                boost::replace_all(opKey, "style.colors.", "");
                boost::trim_right(opKey);
                boost::trim_right(opValue);

                try {
                    if (opKey == "Font")
                        Settings::Instance()->UIFontFile = opValue;
                    else if (opKey == "FontSize")
                        Settings::Instance()->UIFontSize = std::stof(opValue);
                    else if (opKey == "AntiAliasedLines")
                        style.AntiAliasedLines = std::stoi(opValue) != 0;
                    else if (opKey == "AntiAliasedShapes")
                        style.AntiAliasedShapes = std::stoi(opValue) != 0;
                    else if (opKey == "CurveTessellationTol")
                        style.CurveTessellationTol = std::stof(opValue);
                    else if (opKey == "Alpha")
                        style.Alpha = std::stof(opValue);
                    else if (opKey == "WindowFillAlphaDefault")
                        style.WindowFillAlphaDefault = std::stof(opValue);

                    else if (opKey == "WindowPadding")
                        style.WindowPadding = this->tov2(opValue);
                    else if (opKey == "WindowRounding")
                        style.WindowRounding = std::stof(opValue);
                    else if (opKey == "ChildWindowRounding")
                        style.ChildWindowRounding = std::stof(opValue);
                    else if (opKey == "FramePadding")
                        style.FramePadding = this->tov2(opValue);
                    else if (opKey == "FrameRounding")
                        style.FrameRounding = std::stof(opValue);
                    else if (opKey == "ItemSpacing")
                        style.ItemSpacing = this->tov2(opValue);
                    else if (opKey == "ItemInnerSpacing")
                        style.ItemInnerSpacing = this->tov2(opValue);
                    else if (opKey == "TouchExtraPadding")
                        style.TouchExtraPadding = this->tov2(opValue);
                    else if (opKey == "IndentSpacing")
                        style.IndentSpacing = std::stof(opValue);
                    else if (opKey == "ScrollbarSize")
                        style.ScrollbarSize = std::stof(opValue);
                    else if (opKey == "ScrollbarRounding")
                        style.ScrollbarRounding = std::stof(opValue);
                    else if (opKey == "GrabMinSize")
                        style.GrabMinSize = std::stof(opValue);
                    else if (opKey == "GrabRounding")
                        style.GrabRounding = std::stof(opValue);

                    else
                        style.Colors[std::stoi(opKey)] = this->tov4(opValue);
                }
                catch (...) {
                    Settings::Instance()->funcDoLog("[GUI Style] Can't load default GUI styles - [" + opKey + "] with value [" + opValue + "]!");
                }
            }

            fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
        }
    }

    return style;
}

ImGuiStyle& DialogStyle::loadDefault() {
    return this->load(Settings::Instance()->appFolder() + "/KuplungStyleDefault.style");
}

ImGuiStyle& DialogStyle::loadCurrent() {
    return this->load(Settings::Instance()->appFolder() + "/KuplungStyle.style");
}

void DialogStyle::saveStyles(std::string fontfile, std::string fontsize, std::string styleFilePath, ImGuiStyle& style) {
    std::string style_txt;

    style_txt += "# Kuplung (ImGui) styles" + Settings::Instance()->newLineDelimiter;
    style_txt += "# You can export/import style between different applications/users" + Settings::Instance()->newLineDelimiter;
    style_txt += Settings::Instance()->newLineDelimiter;

    style_txt += "# Font" + Settings::Instance()->newLineDelimiter;
    style_txt += "Font = " + fontfile + Settings::Instance()->newLineDelimiter;
    style_txt += "FontSize = " + fontsize + Settings::Instance()->newLineDelimiter;
    style_txt += Settings::Instance()->newLineDelimiter;

    style_txt += "# Rendering" + Settings::Instance()->newLineDelimiter;
    style_txt += "AntiAliasedLines = " + std::to_string(style.AntiAliasedLines) + Settings::Instance()->newLineDelimiter;
    style_txt += "AntiAliasedShapes = " + std::to_string(style.AntiAliasedShapes) + Settings::Instance()->newLineDelimiter;
    style_txt += "CurveTessellationTol = " + std::to_string(style.CurveTessellationTol) + Settings::Instance()->newLineDelimiter;
    style_txt += "Alpha = " + std::to_string(style.Alpha) + Settings::Instance()->newLineDelimiter;
    style_txt += "WindowFillAlphaDefault = " + std::to_string(style.WindowFillAlphaDefault) + Settings::Instance()->newLineDelimiter;
    style_txt += Settings::Instance()->newLineDelimiter;

    style_txt += "# Sizes" + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.WindowPadding = " + std::to_string(style.WindowPadding.x) + ", " + std::to_string(style.WindowPadding.y) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.WindowRounding = " + std::to_string(style.WindowRounding) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.ChildWindowRounding = " + std::to_string(style.ChildWindowRounding) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.FramePadding = " + std::to_string(style.FramePadding.x) + ", " + std::to_string(style.FramePadding.y) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.FrameRounding = " + std::to_string(style.FrameRounding) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.ItemSpacing = " + std::to_string(style.ItemSpacing.x) + ", " + std::to_string(style.ItemSpacing.y) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.ItemInnerSpacing = " + std::to_string(style.ItemInnerSpacing.x) + ", " + std::to_string(style.ItemInnerSpacing.y) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.TouchExtraPadding = " + std::to_string(style.TouchExtraPadding.x) + ", " + std::to_string(style.TouchExtraPadding.y) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.IndentSpacing = " + std::to_string(style.IndentSpacing) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.ScrollbarSize = " + std::to_string(style.ScrollbarSize) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.ScrollbarRounding = " + std::to_string(style.ScrollbarRounding) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.GrabMinSize = " + std::to_string(style.GrabMinSize) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.sizes.GrabRounding = " + std::to_string(style.GrabRounding) + Settings::Instance()->newLineDelimiter;
    style_txt += Settings::Instance()->newLineDelimiter;

    style_txt += "# Colors" + Settings::Instance()->newLineDelimiter;
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
        //style_txt += "style.Colors[ImGuiCol_" + std::string(ImGui::GetStyleColName(i)) + " = ";
        style_txt += "style.colors." + std::to_string(i) + " = ";
        style_txt += std::to_string(style.Colors[i].x) + ", " + std::to_string(style.Colors[i].y) + ", " + std::to_string(style.Colors[i].z) + ", " + std::to_string(style.Colors[i].w) + Settings::Instance()->newLineDelimiter;
    }

    std::ofstream out(styleFilePath);
    out << style_txt;
    out.close();
}

std::vector<std::string> DialogStyle::splitString(const std::string &s, std::regex delimiter) {
    std::vector<std::string> elements;
    std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter) {
        elements.push_back(*iter);
    }
    return elements;
}

ImVec4 DialogStyle::tov4(std::string opValue) {
    std::vector<std::string> values = this->splitString(opValue, std::regex(","));
    return ImVec4(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]), std::stof(values[3]));
}

ImVec2 DialogStyle::tov2(std::string opValue) {
    std::vector<std::string> values = this->splitString(opValue, std::regex(","));
    return ImVec2(std::stof(values[0]), std::stof(values[1]));
}
