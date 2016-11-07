//
//  ExportOBJ.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ExportOBJ.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include <ctime>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

void ExportOBJ::init(int positionX, int positionY, int width, int height, std::function<void(FBEntity, std::vector<std::string>)> saveFile) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
    this->funcFileSave = saveFile;
    this->panelWidth_FileOptions = 200.0f;
    this->panelWidth_FileOptionsMin = 200.0f;
    this->currentFolder = Settings::Instance()->currentFolder;
    this->showNewFolderModel = false;
}

void ExportOBJ::draw(const char* title, bool* p_opened) {
    if (this->width > 0 && this->height > 0)
        ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiSetCond_FirstUseEver);
    else
        ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height), ImGuiSetCond_FirstUseEver);

    if (this->positionX > 0 && this->positionY > 0)
        ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiSetCond_FirstUseEver);

    ImGui::Begin(title, p_opened);
    ImGui::Text("%s", this->currentFolder.c_str());
    ImGui::Separator();

    // file options
    ImGui::BeginChild("OptionsPanel", ImVec2(this->panelWidth_FileOptions, 0));
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
    if (ImGui::Button("To Blender", ImVec2(-1.0f, 0.0f))) {
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
        this->panelWidth_FileOptions += ImGui::GetIO().MouseDelta.x;
        if (this->panelWidth_FileOptions < this->panelWidth_FileOptionsMin)
            this->panelWidth_FileOptions = this->panelWidth_FileOptionsMin;
    }
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(4);

    ImGui::SameLine();

    // folder browser
    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.70f);
    ImGui::Text("File Name: ");
    ImGui::InputText("", this->fileName, sizeof(this->fileName));
    ImGui::SameLine(0, 10);
    if (ImGui::Button("Save")) {
        FBEntity file;

        file.isFile = true;
        file.title = std::string(this->fileName);
        file.path = this->currentFolder + "/" + file.title;
        file.extension = file.title.substr(file.title.rfind(".") + 1);
        file.modifiedDate = "";
        file.size = "";
        Settings::Instance()->currentFolder = this->currentFolder;
        std::vector<std::string> setts;
        setts.push_back(std::to_string(this->Setting_Forward));
        setts.push_back(std::to_string(this->Setting_Up));
        this->funcFileSave(file, setts);
    }
    ImGui::SameLine(0, 10);
    if (ImGui::Button("New Folder"))
        this->showNewFolderModel = true;
    ImGui::PopItemWidth();
    ImGui::Separator();

    if (this->showNewFolderModel)
        this->modalNewFolder();

    // Basic columns
    ImGui::Columns(3, "fileColumns");

    ImGui::Separator();
    ImGui::Text("File");
    ImGui::NextColumn();
    ImGui::Text("Size");
    ImGui::NextColumn();
    ImGui::Text("Last Modified");
    ImGui::NextColumn();
    ImGui::Separator();

    this->drawFiles();

    ImGui::Columns(1);

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}

#pragma mark - Private

void ExportOBJ::modalNewFolder() {
    ImGui::OpenPopup("New Folder");
    ImGui::BeginPopupModal("New Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Folder name:");

    if (this->newFolderName[0] == '\0')
        strcpy(this->newFolderName, "untitled");
    ImGui::InputText("", this->newFolderName, sizeof(this->newFolderName));

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f, 0))) {
        std::string newDir = this->currentFolder + "/" + this->newFolderName;
        if (!boost::filesystem::exists(newDir)) {
            boost::filesystem::path dir(newDir);
            if (!boost::filesystem::create_directory(dir))
                Settings::Instance()->funcDoLog("[FileSaver] Cannot create new folder!");
        }
        ImGui::CloseCurrentPopup();
        this->showNewFolderModel = false;
        this->newFolderName[0] = '\0';
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
        ImGui::CloseCurrentPopup();
        this->showNewFolderModel = false;
        this->newFolderName[0] = '\0';
    }

    ImGui::EndPopup();
}

void ExportOBJ::drawFiles() {
    std::map<std::string, FBEntity> folderContents = this->getFolderContents(this->currentFolder);
    int i = 0;
    static int selected = -1;
    for (std::map<std::string, FBEntity>::iterator iter = folderContents.begin(); iter != folderContents.end(); ++iter) {
        FBEntity entity = iter->second;
        if (ImGui::Selectable(entity.title.c_str(), selected == i, ImGuiSelectableFlags_SpanAllColumns)) {
            selected = i;
            if (entity.isFile)
                strcpy(this->fileName, entity.title.c_str());
            else
                this->currentFolder = entity.path;
            this->drawFiles();
        }
        ImGui::NextColumn();
        ImGui::Text("%s", entity.size.c_str()); ImGui::NextColumn();
        ImGui::Text("%s", entity.modifiedDate.c_str()); ImGui::NextColumn();
        i += 1;
    }
}

std::map<std::string, FBEntity> ExportOBJ::getFolderContents(std::string const& filePath) {
    std::map<std::string, FBEntity> folderContents;

    fs::path currentPath(filePath);

    if (fs::is_directory(currentPath)) {
        this->currentFolder = currentPath.string();

        if (currentPath.has_parent_path()) {
            FBEntity entity;
            entity.isFile = false;
            entity.title = "..";
            entity.path = currentPath.parent_path().string();
            entity.size = "";
            folderContents[".."] = entity;
        }

        fs::directory_iterator iteratorEnd;
        for (fs::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
            try {
                fs::file_status fileStatus = iteratorFolder->status();
                if (!this->isHidden(iteratorFolder->path())) {
                    FBEntity entity;
                    if (fs::is_directory(fileStatus))
                        entity.isFile = false;
                    else if (fs::is_regular_file(fileStatus))
                        entity.isFile = true;
                    else
                        entity.isFile = false;

                    entity.title = iteratorFolder->path().filename().string();
                    if (!entity.isFile)
                        entity.title = "<" + entity.title + ">";

                    entity.extension = iteratorFolder->path().extension().string();

                    entity.path = iteratorFolder->path().string();

                    if (!entity.isFile)
                        entity.size = "";
                    else {
//                        std::string size = boost::lexical_cast<std::string>(fs::file_size(iteratorFolder->path()));
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
                Settings::Instance()->funcDoLog("[SceneExport] " + iteratorFolder->path().filename().string() + " " + ex.what());
            }
        }
    }

    return folderContents;
}

std::string ExportOBJ::convertToString(double num) {
    std::ostringstream convert;
    convert << num;
    return convert.str();
}

std::string ExportOBJ::convertSize(size_t size) {
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

double ExportOBJ::roundOff(double n) {
    double d = n * 100.0;
    int i = d + 0.5;
    d = (float)i / 100.0;
    return d;
}

bool ExportOBJ::isHidden(const fs::path &p) {
    std::string name = p.filename().string();
    if (name == ".." || name == "."  || boost::starts_with(name, "."))
       return true;
    return false;
}