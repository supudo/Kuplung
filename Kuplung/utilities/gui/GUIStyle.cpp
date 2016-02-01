//
//  GUIStyle.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "GUIStyle.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include "utilities/settings/Settings.h"

void GUIStyle::init(std::function<void(std::string)> doLog) {
    this->doLog = doLog;
}

void GUIStyle::saveDefault(ImGuiStyle& style) {
    std::string defaultStyleFile = Settings::Instance()->appFolder() + "/KuplungStyleDefault.style";
    if (!boost::filesystem::exists(defaultStyleFile))
        this->saveStyles(defaultStyleFile, style);
}

void GUIStyle::save(ImGuiStyle& style) {
    this->saveStyles(Settings::Instance()->appFolder() + "/KuplungStyle.style", style);
}

void GUIStyle::load(std::string styleFilePath) {
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

                try {
                    // style.sizes.TouchExtraPadding = 0.000000, 0.000000
                    // style.sizes.IndentSpacing = 22.000000
                    // style.Colors[ImGuiCol_Text] = 0.900000, 0.900000, 0.900000, 1.000000

                    // rendering
                    std::string style_k = opKey;
                    boost::replace_all(style_k, "style.rendering.", "");
                }
                catch (...) {
                    this->doLog("[GUI Style] Can't load default GUI styles!");
                }
            }

            fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
        }
    }
}

void GUIStyle::loadDefault() {
    this->load(Settings::Instance()->appFolder() + "/KuplungStyleDefault.style");
}

void GUIStyle::saveStyles(std::string styleFilePath, ImGuiStyle& style) {
    std::string style_txt;

    style_txt += "# Kuplung (ImGui) styles" + Settings::Instance()->newLineDelimiter;
    style_txt += "# You can export/import style between different applications/users" + Settings::Instance()->newLineDelimiter;
    style_txt += Settings::Instance()->newLineDelimiter;
    style_txt += Settings::Instance()->newLineDelimiter;

    style_txt += "# Rendering" + Settings::Instance()->newLineDelimiter;
    style_txt += "style.rendering.AntiAliasedLines = " + std::to_string(style.AntiAliasedLines) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.rendering.AntiAliasedShapes = " + std::to_string(style.AntiAliasedShapes) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.rendering.CurveTessellationTol = " + std::to_string(style.CurveTessellationTol) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.rendering.Alpha = " + std::to_string(style.Alpha) + Settings::Instance()->newLineDelimiter;
    style_txt += "style.rendering.WindowFillAlphaDefault = " + std::to_string(style.WindowFillAlphaDefault) + Settings::Instance()->newLineDelimiter;
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
        style_txt += "style.Colors[ImGuiCol_" + std::string(ImGui::GetStyleColName(i)) + "] = ";
        style_txt += std::to_string(style.Colors[i].x) + ", " + std::to_string(style.Colors[i].y) + ", " + std::to_string(style.Colors[i].z) + ", " + std::to_string(style.Colors[i].w) + Settings::Instance()->newLineDelimiter;
    }

    std::ofstream out(styleFilePath);
    out << style_txt;
    out.close();
}

std::vector<std::string> GUIStyle::splitString(const std::string &s, std::regex delimiter) {
    std::vector<std::string> elements;
    std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        elements.push_back(*iter);
    return elements;
}
