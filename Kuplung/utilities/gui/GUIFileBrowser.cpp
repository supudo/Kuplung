//
//  GUIFileBrowser.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "GUIFileBrowser.hpp"
#include <ctime>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

void GUIFileBrowser::init(bool log, int positionX, int positionY, int width, int height, std::function<void(std::string)> doLog, std::function<void(FBEntity)> processFile) {
    this->log = log;
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
    this->doLog = doLog;
    this->processFile = processFile;
}

void GUIFileBrowser::draw(const char* title, bool* p_opened) {
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
    ImGui::End();
}

#pragma mark - Private

void GUIFileBrowser::drawFiles() {
    std::map<std::string, FBEntity> folderContents = this->getFolderContents(Settings::Instance()->currentFolder);
    int i = 0;
    static int selected = -1;
    for (std::map<std::string, FBEntity>::iterator iter = folderContents.begin(); iter != folderContents.end(); ++iter) {
        std::string filePath = iter->first;
        FBEntity entity = iter->second;
        
        char label[32];
        sprintf(label, "%i", i);
        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns)) {
            selected = i;
            if (entity.isFile)
                this->processFile(entity);
            else {
                Settings::Instance()->currentFolder = entity.path;
                this->drawFiles();
            }
        }
        ImGui::NextColumn();
        
        std::string allowedExtensions[] = {".obj"};
        bool isAllowedExtension = std::find(std::begin(allowedExtensions), std::end(allowedExtensions), entity.extension) != std::end(allowedExtensions);
        if (isAllowedExtension) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", entity.title.c_str()); ImGui::NextColumn();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", entity.size.c_str()); ImGui::NextColumn();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", entity.modifiedDate.c_str()); ImGui::NextColumn();
        }
        else {
            ImGui::Text("%s", entity.title.c_str()); ImGui::NextColumn();
            ImGui::Text("%s", entity.size.c_str()); ImGui::NextColumn();
            ImGui::Text("%s", entity.modifiedDate.c_str()); ImGui::NextColumn();
        }
        
        i += 1;
    }
}

std::map<std::string, FBEntity> GUIFileBrowser::getFolderContents(std::string filePath) {
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
            entity.size = "";
            folderContents[".."] = entity;
        }

        fs::directory_iterator iteratorEnd;
        for (fs::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
            try {
                bool dotFile = iteratorFolder->path().filename().string().compare(0, 1, ".") == 0;
                fs::file_status fileStatus = iteratorFolder->status();
                if ((fs::is_directory(fileStatus) || fs::is_regular_file(fileStatus)) && !dotFile) {
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
                    
                    if (entity.isFile)
                        this->logMessage(entity.title);
                    else
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

std::string GUIFileBrowser::convertToString(double num) {
    std::ostringstream convert;
    convert << num;
    return convert.str();
}

std::string GUIFileBrowser::convertSize(size_t size) {
    static const char *SIZES[] = { "B", "KB", "MB", "GB" };
    int div = 0;
    size_t rem = 0;
    
    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }
    
    double size_d = (float)size + (float)rem / 1024.0;
    std::string result = this->convertToString(roundOff(size_d)) + " " + SIZES[div];
    return result;
}

double GUIFileBrowser::roundOff(double n) {
    double d = n * 100.0;
    int i = d + 0.5;
    d = (float)i / 100.0;
    return d;
}

void GUIFileBrowser::logMessage(std::string logMessage) {
    if (this->log)
        this->doLog(logMessage);
}