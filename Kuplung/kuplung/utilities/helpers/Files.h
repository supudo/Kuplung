//
//  Files.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 6/3/25.
//  Copyright © 2025 supudo.net. All rights reserved.
//

#ifndef Files_h
#define Files_h

#include "DateTimes.h"
#include <filesystem>
#include <string>

namespace KuplungApp::Helpers {
  static inline bool isHidden(const std::string& fileName) {
    return fileName == ".." || fileName == "." || fileName.compare(0, 1, ".");
  }

  static inline std::string getFilename(const std::string& file) {
    return file.substr(file.find_last_of("/\\") + 1);
  }

  static inline const double roundOff(double n) {
    double d = n * 100.0;
    const int i = static_cast<int>(d + 0.5);
    d = static_cast<double>(i / 100.0);
    return d;
  }

  static inline const std::string convertToString(double num) {
    std::ostringstream convert;
    convert << num;
    return convert.str();
  }

  static inline const std::string convertSize(size_t size) {
    static const char* SIZES[] = {"B", "KB", "MB", "GB"};
    int div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < static_cast<int>(sizeof SIZES / sizeof *SIZES)) {
      rem = (size % 1024);
      div++;
      size /= 1024;
    }

    double size_d = static_cast<double>(size + rem / 1024.0f);
    std::string result = convertToString(roundOff(size_d)) + " " + SIZES[div];
    return result;
  }

  static inline std::map<std::string, FBEntity> getFolderContents(std::string const& filePath) {
    std::map<std::string, FBEntity> folderContents;

    std::filesystem::path currentPath(filePath);

    if (std::filesystem::is_directory(currentPath)) {
      if (currentPath.has_parent_path()) {
        FBEntity entity;
        entity.isFile = false;
        entity.title = "..";
        entity.path = currentPath.parent_path().string();
        entity.size.clear();
        folderContents[".."] = entity;
      }

      std::filesystem::directory_iterator iteratorEnd;
      for (std::filesystem::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
        try {
          std::filesystem::file_status fileStatus = iteratorFolder->status();
          FBEntity entity;
          if (std::filesystem::is_directory(fileStatus))
            entity.isFile = false;
          else if (std::filesystem::is_regular_file(fileStatus))
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
          else
            entity.size = convertSize(std::filesystem::file_size(iteratorFolder->path()));

          entity.modifiedDate = KuplungApp::Helpers::getDateToStringFormatted(std::filesystem::last_write_time(iteratorFolder->path()).time_since_epoch(), "%Y-%m-%d %H:%M:%S");

          folderContents[entity.path] = entity;
        } catch (const std::exception& ex) {
          Settings::Instance()->funcDoLog("[SceneExport] " + iteratorFolder->path().filename().string() + " " + ex.what());
        }
      }
    }

    return folderContents;
  }
} // namespace KuplungApp::Helpers

#endif /* Files_h */
