//
//  FileSaver.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/FileSaver.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include <ctime>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

void FileSaver::init(int positionX, int positionY, int width, int height, const std::function<void(FBEntity, FileSaverOperation)>& saveFile) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
    this->funcFileSave = saveFile;
    this->panelWidth_FileOptions = 200.0f;
    this->currentFolder = Settings::Instance()->currentFolder;
    this->showNewFolderModel = false;
}

void FileSaver::draw(const char* title, FileSaverOperation type, bool* p_opened) {
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
    ImGui::BeginChild("file_options", ImVec2(this->panelWidth_FileOptions, 0));
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
    ImGui::Text("Options");
#ifdef _WIN32
	ImGui::Separator();
	ImGui::Text("Select Drive");
	ImGui::Combo(
		"##8820",
		&Settings::Instance()->Setting_SelectedDriveIndex,
		[](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size()))
			return false;
		*out_text = vector.at(idx).c_str();
		return true;
	},
		static_cast<void*>(&Settings::Instance()->hddDriveList),
		Settings::Instance()->hddDriveList.size()
		);
	ImGui::Separator();
#endif
    ImGui::PopItemWidth();
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(89, 91, 94));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(119, 122, 124));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0));
    ImGui::Button("###splitterOptions", ImVec2(2.0f, -1));
    ImGui::PopStyleColor(3);
    ImGui::SameLine();

    // folder browser
    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.70f);
    ImGui::Text("File Name: ");
    ImGui::InputText("", this->fileName, sizeof(this->fileName));
    ImGui::SameLine(0, 10);
    std::string btnLabel = "Save";
    if (type == FileSaverOperation_OpenScene)
        btnLabel = "Open";
    if (ImGui::Button(btnLabel.c_str())) {
        FBEntity file;

        file.isFile = true;
        file.title = std::string(this->fileName);
        file.path = this->currentFolder + "/" + file.title;
		file.extension = file.title.substr(file.title.rfind('.') + 1);

//        std::time_t modifiedDate = fs::last_write_time(file.path);
//        std::tm* modifiedDateLocal = std::localtime(&modifiedDate);
//        std::string mds = std::to_string((modifiedDateLocal->tm_year + 1900));
//        mds += "-" + std::to_string((modifiedDateLocal->tm_mon + 1));
//        mds += "-" + std::to_string(modifiedDateLocal->tm_mday);
//        mds += " " + std::to_string(modifiedDateLocal->tm_hour);
//        mds += ":" + std::to_string(modifiedDateLocal->tm_min);
//        mds += "." + std::to_string(modifiedDateLocal->tm_sec);
//        file.modifiedDate = std::move(mds);
        file.modifiedDate.clear();

//        file.size = this->convertSize(fs::file_size(file.path));
        file.size.clear();
        Settings::Instance()->currentFolder = this->currentFolder;
        this->funcFileSave(file, type);
    }
    ImGui::SameLine(0, 10);
    if (type != FileSaverOperation_OpenScene && ImGui::Button("New Folder"))
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

    this->drawFiles(this->currentFolder);

    ImGui::Columns(1);

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}

#pragma mark - Private

void FileSaver::modalNewFolder() {
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

void FileSaver::drawFiles(const std::string& fPath) {
	std::string cFolder = fPath;
#ifdef _WIN32
	if (Settings::Instance()->Setting_CurrentDriveIndex != Settings::Instance()->Setting_SelectedDriveIndex) {
		cFolder = Settings::Instance()->hddDriveList[Settings::Instance()->Setting_SelectedDriveIndex] + ":\\";
		Settings::Instance()->Setting_CurrentDriveIndex = Settings::Instance()->Setting_SelectedDriveIndex;
	}
#endif
	std::map<std::string, FBEntity> folderContents = this->getFolderContents(cFolder);
    int i = 0;
    static int selected = -1;
    for (std::map<std::string, FBEntity>::iterator iter = folderContents.begin(); iter != folderContents.end(); ++iter) {
        FBEntity entity = iter->second;
        if (ImGui::Selectable(entity.title.c_str(), selected == i, ImGuiSelectableFlags_SpanAllColumns)) {
            selected = i;
            if (entity.isFile)
                strcpy(this->fileName, entity.title.c_str());
			else {
				try {
					this->drawFiles(entity.path);
					this->currentFolder = entity.path;
				}
				catch (const fs::filesystem_error& e) {}
			}
        }
        ImGui::NextColumn();
        ImGui::Text("%s", entity.size.c_str()); ImGui::NextColumn();
        ImGui::Text("%s", entity.modifiedDate.c_str()); ImGui::NextColumn();
        i += 1;
    }
}

std::map<std::string, FBEntity> FileSaver::getFolderContents(std::string const& filePath) {
    std::map<std::string, FBEntity> folderContents;

    fs::path currentPath(filePath);

    if (fs::is_directory(currentPath)) {
        this->currentFolder = currentPath.string();

        if (currentPath.has_parent_path()) {
            FBEntity entity;
            entity.isFile = false;
            entity.title = "..";
            entity.path = currentPath.parent_path().string();
            entity.size.clear();
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
                        entity.size.clear();
                    else {
//                        std::string size = boost::lexical_cast<std::string>(fs::file_size(iteratorFolder->path()));
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
                }
            }
            catch (const std::exception & ex) {
                Settings::Instance()->funcDoLog("[SceneExport] " + iteratorFolder->path().filename().string() + " " + ex.what());
            }
        }
    }

    return folderContents;
}

std::string FileSaver::convertToString(double num) {
    std::ostringstream convert;
    convert << num;
    return convert.str();
}

std::string FileSaver::convertSize(size_t size) {
    static const char *SIZES[] = { "B", "KB", "MB", "GB" };
    int div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < static_cast<int>(sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    double size_d = static_cast<double>(size + rem / 1024.0f);
    std::string result = this->convertToString(roundOff(size_d)) + " " + SIZES[div];
    return result;
}

double FileSaver::roundOff(double n) {
    double d = n * 100.0;
    const int i = static_cast<int>(d + 0.5);
    d = static_cast<double>(i / 100.0);
    return d;
}

bool FileSaver::isHidden(const fs::path &p) {
    std::string name = p.filename().string();
    if (name == ".." || name == "."  || boost::starts_with(name, "."))
       return true;
    return false;
}
