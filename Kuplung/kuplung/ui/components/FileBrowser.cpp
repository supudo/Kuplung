//
//  GUIFileBrowser.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/FileBrowser.hpp"
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

void FileBrowser::init(bool log, int positionX, int positionY, int width, int height, const std::function<void(FBEntity, MaterialTextureType)>& processFile) {
    this->log = log;
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
    this->processFile = processFile;
    this->isStyleBrowser = false;
}

void FileBrowser::setStyleBrowser(bool isStyle) {
    this->isStyleBrowser = isStyle;
    this->isImageBrowser = false;
}

void FileBrowser::setImageBrowser(bool isImage) {
    this->isImageBrowser = isImage;
    this->isStyleBrowser = false;
}

void FileBrowser::draw(const char* title, bool* p_opened, MaterialTextureType TextureType) {
    if (this->width > 0 && this->height > 0)
        ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiSetCond_FirstUseEver);
    else
        ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height), ImGuiSetCond_FirstUseEver);

    if (this->positionX > 0 && this->positionY > 0)
        ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiSetCond_FirstUseEver);

    ImGui::Begin(title, p_opened);
    ImGui::Text("Select OBJ file");
    ImGui::Separator();
    ImGui::Text("%s", Settings::Instance()->currentFolder.c_str());
    ImGui::Separator();

    ImGui::Text("Mode File Parser:"); ImGui::SameLine();
#ifdef DEF_KuplungSetting_UseCuda
    const char* parserItems[] = {"Kuplung Parsers", "Kuplung Parsers - Cuda", "Assimp"};
#else
    const char* parserItems[] = {"Kuplung Parsers", "Assimp"};
#endif
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
    ImGui::Text("File");
    ImGui::NextColumn();
    ImGui::Text("Size");
    ImGui::NextColumn();
    ImGui::Text("Last Modified");
    ImGui::NextColumn();
    ImGui::Separator();

    ImGui::SetColumnOffset(1, 40);

    this->drawFiles(TextureType);

    ImGui::Columns(1);

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}

#pragma mark - Private

void FileBrowser::drawFiles(MaterialTextureType TextureType) {
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
                this->processFile(entity, TextureType);

#ifdef _WIN32
        std::string folderDelimiter = "\\";
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
                this->drawFiles(TextureType);
            }
        }
        ImGui::NextColumn();

        ImGui::Text("%s", entity.title.c_str()); ImGui::NextColumn();
        ImGui::Text("%s", entity.size.c_str()); ImGui::NextColumn();
        ImGui::Text("%s", entity.modifiedDate.c_str()); ImGui::NextColumn();

        i += 1;
    }
}

std::map<std::string, FBEntity> FileBrowser::getFolderContents(std::string const& filePath) {
    std::map<std::string, FBEntity> folderContents;

    if (this->log)
        this->logMessage("-- Listing folder contents : " + filePath);
    fs::path currentPath(filePath);

    if (fs::is_directory(currentPath)) {
        Settings::Instance()->currentFolder = currentPath.string();

        if (currentPath.has_parent_path()) {
            FBEntity entity;
            entity.isFile = false;
            entity.title = "..";
            entity.path = currentPath.parent_path().string();
            entity.size.clear();
            folderContents[".."] = entity;
        }

        fs::directory_iterator iteratorEnd;
        bool isAllowedFileExtension = false;
        for (fs::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
            try {
                fs::file_status fileStatus = iteratorFolder->status();
                isAllowedFileExtension = Settings::Instance()->isAllowedFileExtension(iteratorFolder->path().extension().string());
                if (this->isStyleBrowser)
                    isAllowedFileExtension = Settings::Instance()->isAllowedStyleExtension(iteratorFolder->path().extension().string());
                else if (this->isImageBrowser)
                    isAllowedFileExtension = Settings::Instance()->isAllowedImageExtension(iteratorFolder->path().extension().string());
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
                        entity.size.clear();
                    else {
                        //std::string size = boost::lexical_cast<std::string>(fs::file_size(iteratorFolder->path()));
                        entity.size = this->convertSize(fs::file_size(iteratorFolder->path()));
                    }

                    std::time_t modifiedDate = fs::last_write_time(iteratorFolder->path());
                    const std::tm* modifiedDateLocal = std::localtime(&modifiedDate);
                    std::string mds = std::to_string((modifiedDateLocal->tm_year + 1900));
                    mds += "-" + std::to_string((modifiedDateLocal->tm_mon + 1));
                    mds += "-" + std::to_string(modifiedDateLocal->tm_mday);
                    mds += " " + std::to_string(modifiedDateLocal->tm_hour);
                    mds += ":" + std::to_string(modifiedDateLocal->tm_min);
                    mds += "." + std::to_string(modifiedDateLocal->tm_sec);
                    entity.modifiedDate = std::move(mds);

                    folderContents[entity.path] = entity;

                    this->logMessage(entity.title);
                }
            }
            catch (const std::exception & ex) {
                this->logMessage(iteratorFolder->path().filename().string() + " " + ex.what());
            }
        }
    }
    this->logMessage("-- Folder contents end.");

    return folderContents;
}

std::string FileBrowser::convertToString(double num) {
    std::ostringstream convert;
    convert << num;
    return convert.str();
}

std::string FileBrowser::convertSize(size_t size) {
    static const char *SIZES[] = { "B", "KB", "MB", "GB" };
    int div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < int((sizeof SIZES / sizeof * SIZES))) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    double size_d = static_cast<double>(size) + static_cast<double>(rem) / 1024.0;
    std::string result = this->convertToString(roundOff(size_d)) + " " + SIZES[div];
    return result;
}

double FileBrowser::roundOff(double n) {
    double d = n * 100.0;
    int i = static_cast<int>(d + 0.5);
    d = static_cast<double>(i) / 100.0;
    return d;
}

void FileBrowser::logMessage(std::string const& logMessage) {
    if (this->log)
        Settings::Instance()->funcDoLog("[FileBrowser] " + logMessage);
}

bool FileBrowser::isHidden(const fs::path &p) {
    std::string name = p.filename().string();
    if (name == ".." || name == "."  || boost::starts_with(name, "."))
       return true;
    return false;
}
