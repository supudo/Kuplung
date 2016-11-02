//
//  ImportOBJ.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ImportOBJ.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include <ctime>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

void ImportOBJ::init(int positionX, int positionY, int width, int height, std::function<void(FBEntity, std::vector<std::string>)> processFile) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
    this->processFile = processFile;
    this->panelWidth_Options = 200.0f;
    this->panelWidth_OptionsMin = 200.0f;
    this->Setting_Forward = 1;
    this->Setting_Up = 5;
}

void ImportOBJ::draw(const char* title, bool* p_opened) {
    if (this->width > 0 && this->height > 0)
        ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiSetCond_FirstUseEver);
    else
        ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height), ImGuiSetCond_FirstUseEver);

    if (this->positionX > 0 && this->positionY > 0)
        ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiSetCond_FirstUseEver);

    ImGui::Begin(title, p_opened);

    ImGui::BeginChild("OptionsPanel", ImVec2(this->panelWidth_Options, 0));
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Options");
    ImGui::Separator();
    ImGui::PushItemWidth(-1.0f);
    ImGui::Text("Forward");
    const char* forward_items[] = {
        "-X Forward",
        "-Y Forward",
        "-Z Forward",
        "X Forward",
        "Y Forward",
        "Z Forward"
    };
    ImGui::Combo("##987", &this->Setting_Forward, forward_items, IM_ARRAYSIZE(forward_items));
    ImGui::Separator();
    ImGui::Text("Up");
    const char* up_items[] = {
        "-X Up",
        "-Y Up",
        "-Z Up",
        "X Up",
        "Y Up",
        "Z Up"
    };
    ImGui::Combo("##988", &this->Setting_Up, up_items, IM_ARRAYSIZE(up_items));
    ImGui::Separator();
    if (ImGui::Button("From Blender", ImVec2(-1.0f, 0.0f))) {
        this->Setting_Forward = 2;
        this->Setting_Up = 4;
    }
    ImGui::PopItemWidth();

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(89, 91, 94));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(119, 122, 124));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0));
    ImGui::Button("###splitterOptions", ImVec2(8.0f, -1));
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemActive()) {
        this->panelWidth_Options += ImGui::GetIO().MouseDelta.x;
        if (this->panelWidth_Options < this->panelWidth_OptionsMin)
            this->panelWidth_Options = this->panelWidth_OptionsMin;
    }
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(4);

    ImGui::SameLine();

    ImGui::BeginChild("Browser", ImVec2(-1.0f, -1.0f), false);

    ImGui::Text("Select OBJ file");
    ImGui::Separator();
    ImGui::Text("%s", Settings::Instance()->currentFolder.c_str());
    ImGui::Separator();

    ImGui::Text("Mode File Parser:"); ImGui::SameLine();
    const char* parserItems[] = {"Kuplung Obj Parser 1.0", "Kuplung Obj Parser 2.0", "Assimp"};
    if (ImGui::Combo("##00392", &Settings::Instance()->ModelFileParser, parserItems, IM_ARRAYSIZE(parserItems)))
        Settings::Instance()->saveSettings();

    ImGui::Separator();

    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

    // Basic columns
    ImGui::Columns(4, "fileColumns");

    ImGui::Separator();
    ImGui::Text("ID");
    ImGui::NextColumn();
    ImGui:: Text("File");
    ImGui::NextColumn();
    ImGui::Text("Size");
    ImGui::NextColumn();
    ImGui::Text("Last Modified");
    ImGui::NextColumn();
    ImGui::Separator();

    ImGui::SetColumnOffset(1, 40);

    this->drawFiles();

    ImGui::Columns(1);

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::EndChild();

    ImGui::End();
}

#pragma mark - Private

void ImportOBJ::drawFiles() {
    std::map<std::string, FBEntity> folderContents = this->getFolderContents(Settings::Instance()->currentFolder);
    int i = 0;
    static int selected = -1;
    for (std::map<std::string, FBEntity>::iterator iter = folderContents.begin(); iter != folderContents.end(); ++iter) {
        FBEntity entity = iter->second;

        char label[32];
        sprintf(label, "%i", i);
        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns)) {
            selected = i;
            if (entity.isFile) {
                std::vector<std::string> setts;
                setts.push_back(std::to_string(this->Setting_Forward));
                setts.push_back(std::to_string(this->Setting_Up));
                this->processFile(entity, setts);

#ifdef _WIN32
        std::string folderDelimiter = "\";
#else
        std::string folderDelimiter = "/";
#endif
                std::vector<std::string> elems;
                boost::split(elems, entity.path, boost::is_any_of(folderDelimiter));
                elems.pop_back();

                Settings::Instance()->currentFolder = boost::algorithm::join(elems, folderDelimiter);
                Settings::Instance()->saveSettings();
            }
            else {
                Settings::Instance()->currentFolder = entity.path;
                this->drawFiles();
            }
        }
        ImGui::NextColumn();

        ImGui::Text("%s", entity.title.c_str()); ImGui::NextColumn();
        ImGui::Text("%s", entity.size.c_str()); ImGui::NextColumn();
        ImGui::Text("%s", entity.modifiedDate.c_str()); ImGui::NextColumn();

        i += 1;
    }
}

std::map<std::string, FBEntity> ImportOBJ::getFolderContents(std::string const& filePath) {
    std::map<std::string, FBEntity> folderContents;
    fs::path currentPath(filePath);

    if (fs::is_directory(currentPath)) {
        Settings::Instance()->currentFolder = currentPath.string();

        if (currentPath.has_parent_path()) {
            FBEntity entity;
            entity.isFile = false;
            entity.title = "..";
            entity.path = currentPath.parent_path().string();
            entity.size = "";
            folderContents[".."] = entity;
        }

        fs::directory_iterator iteratorEnd;
        bool isAllowedFileExtension;
        for (fs::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
            try {
                fs::file_status fileStatus = iteratorFolder->status();
                isAllowedFileExtension = Settings::Instance()->isAllowedFileExtension(iteratorFolder->path().extension().string());
                if (isAllowedFileExtension || (fs::is_directory(fileStatus) && !this->isHidden(iteratorFolder->path()))) {
                    FBEntity entity;
                    if (fs::is_directory(fileStatus))
                        entity.isFile = false;
                    else if (fs::is_regular_file(fileStatus))
                        entity.isFile = true;

                    entity.title = iteratorFolder->path().filename().string();
                    if (!entity.isFile)
                        entity.title = "<" + entity.title + ">";

                    entity.extension = iteratorFolder->path().extension().string();

                    entity.path = iteratorFolder->path().string();

                    if (!entity.isFile)
                        entity.size = "";
                    else {
                        std::string size = boost::lexical_cast<std::string>(fs::file_size(iteratorFolder->path()));
                        entity.size = this->convertSize(fs::file_size(iteratorFolder->path()));
                    }

                    std::time_t modifiedDate = fs::last_write_time(iteratorFolder->path());
                    std::tm* modifiedDateLocal = std::localtime(&modifiedDate);
                    std::string mds = std::to_string((modifiedDateLocal->tm_year + 1900));
                    mds += "-" + std::to_string((modifiedDateLocal->tm_mon + 1));
                    mds += "-" + std::to_string(modifiedDateLocal->tm_mday);
                    mds += " " + std::to_string(modifiedDateLocal->tm_hour);
                    mds += ":" + std::to_string(modifiedDateLocal->tm_min);
                    mds += "." + std::to_string(modifiedDateLocal->tm_sec);
                    entity.modifiedDate = mds;

                    folderContents[entity.path] = entity;
                }
            }
            catch (const std::exception & ex) {
                Settings::Instance()->funcDoLog("[ImportOBJ] " + iteratorFolder->path().filename().string() + " " + ex.what());
            }
        }
    }

    return folderContents;
}

std::string ImportOBJ::convertToString(double num) {
    std::ostringstream convert;
    convert << num;
    return convert.str();
}

std::string ImportOBJ::convertSize(size_t size) {
    static const char *SIZES[] = { "B", "KB", "MB", "GB" };
    int div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (int)(sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    double size_d = (float)size + (float)rem / 1024.0;
    std::string result = this->convertToString(roundOff(size_d)) + " " + SIZES[div];
    return result;
}

double ImportOBJ::roundOff(double n) {
    double d = n * 100.0;
    int i = d + 0.5;
    d = (float)i / 100.0;
    return d;
}

bool ImportOBJ::isHidden(const fs::path &p) {
    std::string name = p.filename().string();
    if (name == ".." || name == "."  || boost::starts_with(name, "."))
       return true;
    return false;
}
