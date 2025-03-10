//
//  ImportFile.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ImportFile.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/utilities/helpers/Helpers.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <format>

namespace fs = std::filesystem;

void ImportFile::init(int positionX, int positionY, int width, int height, const std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats importFormat, int importFormatAssimp)>& processFile) {
  this->positionX = positionX;
  this->positionY = positionY;
  this->width = width;
  this->height = height;
  this->processFile = processFile;
  this->panelWidth_Options = 200.0f;
  this->panelWidth_OptionsMin = 200.0f;
  this->Setting_Forward = 2;
  this->Setting_Up = 4;
	this->currentFolder = Settings::Instance()->currentFolder;
	for (size_t i = 0; i < Settings::Instance()->AssimpSupportedFormats_Import.size(); i++)
		this->assimpImporters.push_back(Settings::Instance()->AssimpSupportedFormats_Import[i].description.c_str());
}

static char *convert(const std::string & s) {
	char *pc = new char[s.size() + 1];
	std::strcpy(pc, s.c_str());
	return pc;
}

void ImportFile::draw(ImportExportFormats* dialogImportType, int* dialogImportType_Assimp, bool* p_opened) {
	std::string window_title = "Import ";
	switch (*dialogImportType) {
		case ImportExportFormat_OBJ:
			window_title = "Wavefront OBJ file";
			break;
		case ImportExportFormat_STL:
			window_title = "STereoLithography STL file";
			break;
		case ImportExportFormat_PLY:
			window_title = "Stanford PLY file";
			break;
		case ImportExportFormat_GLTF:
			window_title = "glTF file";
			break;
	}
	const char* title = window_title.c_str();

	if (this->width > 0 && this->height > 0)
		ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiCond_FirstUseEver);
	else
		ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height), ImGuiCond_FirstUseEver);

	if (this->positionX > 0 && this->positionY > 0)
		ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiCond_FirstUseEver);

	ImGui::Begin("Import file", p_opened);
	ImGui::Text("%s", this->currentFolder.c_str());
	ImGui::Separator();

	// file options
	ImGui::BeginChild("OptionsPanel", ImVec2(this->panelWidth_Options, 0));
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Options");
	ImGui::Separator();
	ImGui::PushItemWidth(-1.0f);
	ImGui::Text("Kuplung File Format");
	const char* file_format_items[] = {
		"Wavefront OBJ",
		"glTF",
		"STereoLithography STL",
		"Stanford PLY"
	};
	ImGui::Combo("##982", (int*)dialogImportType, file_format_items, IM_ARRAYSIZE(file_format_items));
	ImGui::Text("Assimp File Format");
	ImGui::Combo("##983", (int*)dialogImportType_Assimp, &this->assimpImporters[0], int(this->assimpImporters.size()));
	ImGui::PopItemWidth();
	ImGui::Separator();
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
	ImGui::Separator();
	ImGui::Text("Parser:");
#ifdef DEF_KuplungSetting_UseCuda
	const char* parserItems[] = { "Kuplung", "Kuplung Cuda", "Assimp" };
#else
	const char* parserItems[] = { "Kuplung", "Assimp" };
#endif
	if (ImGui::Combo("##00392", &Settings::Instance()->ModelFileParser, parserItems, IM_ARRAYSIZE(parserItems)))
		Settings::Instance()->saveSettings();
	ImGui::PopItemWidth();
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

	// folder browser
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

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

	this->drawFiles(dialogImportType, dialogImportType_Assimp, this->currentFolder);

	ImGui::Columns(1);

	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}

void ImportFile::drawFiles(ImportExportFormats* dialogImportType, int* dialogImportType_Assimp, const std::string& fPath) {
	std::string cFolder = fPath;
#ifdef _WIN32
	if (Settings::Instance()->Setting_CurrentDriveIndex != Settings::Instance()->Setting_SelectedDriveIndex) {
		cFolder = Settings::Instance()->hddDriveList[Settings::Instance()->Setting_SelectedDriveIndex] + ":\\";
		Settings::Instance()->Setting_CurrentDriveIndex = Settings::Instance()->Setting_SelectedDriveIndex;
		this->currentFolder = cFolder;
	}
#endif
	std::map<std::string, FBEntity> folderContents = this->getFolderContents(dialogImportType, cFolder);
	int i = 0;
  static int selected = -1;
  for (std::map<std::string, FBEntity>::iterator iter = folderContents.begin(); iter != folderContents.end(); ++iter) {
    FBEntity entity = iter->second;
    if (ImGui::Selectable(entity.title.c_str(), selected == i, ImGuiSelectableFlags_SpanAllColumns)) {
      selected = i;
      if (entity.isFile) {
        std::vector<std::string> setts;
        setts.push_back(std::to_string(this->Setting_Forward));
        setts.push_back(std::to_string(this->Setting_Up));
        this->processFile(entity, setts, static_cast<ImportExportFormats>(*dialogImportType), *dialogImportType_Assimp);

#ifdef _WIN32
        char folderDelimiter = '\\';
#else
        char folderDelimiter = '/';
#endif
        std::vector<std::string> elems = Kuplung::Helpers::splitString(entity.path, folderDelimiter);
        elems.pop_back();
        Settings::Instance()->currentFolder = Kuplung::Helpers::joinElementsToString(elems, folderDelimiter);
        Settings::Instance()->saveSettings();
      }
      else {
        try {
          this->drawFiles(dialogImportType, dialogImportType_Assimp, entity.path);
          this->currentFolder = entity.path;
        }
        catch (const fs::filesystem_error&) { }
      }
    }
    ImGui::NextColumn();

    ImGui::Text("%s", entity.size.c_str()); ImGui::NextColumn();
    ImGui::Text("%s", entity.modifiedDate.c_str()); ImGui::NextColumn();

    i += 1;
  }
}

std::map<std::string, FBEntity> ImportFile::getFolderContents(ImportExportFormats* dialogImportType, std::string const& filePath) {
  std::map<std::string, FBEntity> folderContents;
  fs::path currentPath(filePath);

	if (fs::is_directory(currentPath)) {
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
				if (*dialogImportType != ImportExportFormat_UNDEFINED) {
					switch (*dialogImportType) {
						case ImportExportFormat_OBJ: {
							isAllowedFileExtension = Settings::Instance()->isAllowedFileExtension(iteratorFolder->path().extension().string(), { ".obj" });
							break;
						}
						case ImportExportFormat_GLTF: {
							isAllowedFileExtension = Settings::Instance()->isAllowedFileExtension(iteratorFolder->path().extension().string(), { ".gltf" });
							break;
						}
						case ImportExportFormat_PLY: {
							isAllowedFileExtension = Settings::Instance()->isAllowedFileExtension(iteratorFolder->path().extension().string(), { ".ply" });
							break;
						}
						case ImportExportFormat_STL: {
							isAllowedFileExtension = Settings::Instance()->isAllowedFileExtension(iteratorFolder->path().extension().string(), { ".stl" });
							break;
						}
					}
				}
				else {
					for (size_t i = 0; i < Settings::Instance()->AssimpSupportedFormats_Import.size(); i++) {
						std::string fe = Settings::Instance()->AssimpSupportedFormats_Import[i].fileExtension;
						std::string ce = iteratorFolder->path().extension().string();
						if (fe.find(ce) != std::string::npos) {
							isAllowedFileExtension = true;
							break;
						}
					}
				}
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
				}
			}
			catch (const std::exception &) { }
		}
	}

  return folderContents;
}

const std::string ImportFile::convertToString(double num) const {
  std::ostringstream convert;
  convert << num;
  return convert.str();
}

const std::string ImportFile::convertSize(size_t size) const {
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

const double ImportFile::roundOff(double n) const {
  double d = n * 100.0;
  int i = d + 0.5;
  d = (float)i / 100.0;
  return d;
}
