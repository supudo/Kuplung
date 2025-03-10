//
//  GUIFileBrowser.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/FileBrowser.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/utilities/helpers/Helpers.h"
#include <filesystem>
#include <ctime>
#include <iostream>
#include <sstream>
#include <format>

namespace fs = std::filesystem;

void FileBrowser::init(bool log, int positionX, int positionY, int width, int height, const std::function<void(FBEntity, MaterialTextureType)>& processFile) {
  this->log = log;
  this->positionX = positionX;
  this->positionY = positionY;
  this->width = width;
  this->height = height;
  this->processFile = processFile;
  this->isStyleBrowser = false;
  this->panelWidth_Options = 200.0f;
  this->panelWidth_OptionsMin = 200.0f;
  this->currentFolder = Settings::Instance()->currentFolder;
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
    ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiCond_FirstUseEver);
  else
    ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height), ImGuiCond_FirstUseEver);

  if (this->positionX > 0 && this->positionY > 0)
    ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiCond_FirstUseEver);

  ImGui::Begin(title, p_opened);

  ImGui::BeginChild("OptionsPanel", ImVec2(this->panelWidth_Options, 0));
  ImGui::TextColored(ImVec4(1, 0, 0, 1), "Options");
#ifdef _WIN32
  ImGui::Separator();
  ImGui::Text("Select Drive");
  ImGui::Combo(
      "##8820",
      &Settings::Instance()->Setting_SelectedDriveIndex,
      [](void* vec, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::string>*>(vec);
        if (idx < 0 || idx >= static_cast<int>(vector.size()))
          return false;
        *out_text = vector.at(idx).c_str();
        return true;
      },
      static_cast<void*>(&Settings::Instance()->hddDriveList),
      Settings::Instance()->hddDriveList.size());
  ImGui::Separator();
#endif
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::GetIO().MouseDrawCursor = true;
  ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(89, 91, 94)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(119, 122, 124)));
  ImGui::PushStyleColor(ImGuiCol_Border, static_cast<ImVec4>(ImColor(0, 0, 0)));
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

  ImGui::Text("Select Texture");
  ImGui::Separator();
  ImGui::Text("%s", this->currentFolder.c_str());
  ImGui::Separator();

  ImGui::Text("Mode File Parser:");
  ImGui::SameLine();
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

  this->drawFiles(this->currentFolder, TextureType);

  ImGui::Columns(1);

  ImGui::Separator();
  ImGui::Spacing();

  ImGui::PopStyleVar();
  ImGui::EndChild();

  ImGui::EndChild();

  ImGui::End();
}

void FileBrowser::drawFiles(const std::string& fPath, MaterialTextureType TextureType) {
  std::string cFolder = fPath;
#ifdef _WIN32
  if (Settings::Instance()->Setting_CurrentDriveIndex != Settings::Instance()->Setting_SelectedDriveIndex) {
    cFolder = Settings::Instance()->hddDriveList[Settings::Instance()->Setting_SelectedDriveIndex] + ":\\";
    Settings::Instance()->Setting_CurrentDriveIndex = Settings::Instance()->Setting_SelectedDriveIndex;
    this->currentFolder = cFolder;
  }
#endif
  std::map<std::string, FBEntity> folderContents = this->getFolderContents(cFolder);
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
        char folderDelimiter = '\\';
#else
        char folderDelimiter = "/";
#endif
        std::vector<std::string> elems = Kuplung::Helpers::splitString(entity.path, folderDelimiter);
        elems.pop_back();
        Settings::Instance()->currentFolder = Kuplung::Helpers::joinElementsToString(elems, folderDelimiter);
        Settings::Instance()->saveSettings();
      }
      else {
        try {
          this->drawFiles(entity.path, TextureType);
          this->currentFolder = entity.path;
        } catch (const fs::filesystem_error&) {
        }
      }
    }
    ImGui::NextColumn();

    ImGui::Text("%s", entity.title.c_str());
    ImGui::NextColumn();
    ImGui::Text("%s", entity.size.c_str());
    ImGui::NextColumn();
    ImGui::Text("%s", entity.modifiedDate.c_str());
    ImGui::NextColumn();

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
        if (isAllowedFileExtension || (fs::is_directory(fileStatus) && !Kuplung::Helpers::isHidden(iteratorFolder->path().string()))) {
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
          else
            entity.size = this->convertSize(fs::file_size(iteratorFolder->path()));

          entity.modifiedDate = Kuplung::Helpers::getDateToStringFormatted(fs::last_write_time(iteratorFolder->path()).time_since_epoch(), "%Y-%m-%d %H:%M:%S");

          folderContents[entity.path] = entity;

          this->logMessage(entity.title);
        }
      } catch (const std::exception& ex) {
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
  static const char* SIZES[] = {"B", "KB", "MB", "GB"};
  int div = 0;
  size_t rem = 0;

  while (size >= 1024 && div < int((sizeof SIZES / sizeof *SIZES))) {
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
