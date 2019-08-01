//
//  FontsList.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/4/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "FontsList.hpp"
#include <algorithm>
#include <stdio.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "kuplung/settings/Settings.h"

#ifdef WIN32
#include <shlobj.h>
#endif

namespace fs = boost::filesystem;

void FontsList::init() {
  this->fonts.clear();
}

bool FontsList::fontFileExists(std::string const& font) {
  return boost::filesystem::exists(font);
}

int FontsList::getSelectedFontSize() {
  for (size_t i=0; i<sizeof(this->fontSizes[0]); i++) {
    if (atof(this->fontSizes[i]) < Settings::Instance()->UIFontSize)
      return static_cast<int>(i);
  }
  return 0;
}

void FontsList::getFonts() {
#ifdef WIN32
  return this->loadFontsWindows();
#elif __APPLE__
  return this->loadFontsOSX();
#elif linux || __linux
  return this->loadFontsNix();
#endif
}

void FontsList::loadFontsOSX() {
  //| /System/Library/Fonts - Fonts necessary for the system. Do not touch these.
  //| /Library/Fonts - Additional fonts that can be used by all users. This is generally where fonts go if they are to be used by other applications.
  //| ~/Library/Fonts - Fonts specific to each user.
  //| /Network/Library/Fonts - Fonts shared for users on a network.

  std::string fontsAdditionalPath = "/Library/Fonts/";
  fs::path currentPath(fontsAdditionalPath);

  if (fs::is_directory(currentPath)) {
    fs::directory_iterator iteratorEnd;
    int i = 1;
    for (fs::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
      try {
        std::string fileExtension = iteratorFolder->path().extension().string();
        fs::file_status fileStatus = iteratorFolder->status();
        if (fileExtension == ".ttf" && (fs::is_directory(fileStatus) || fs::is_regular_file(fileStatus))) {
          FBEntity fontFile;
          fontFile.isFile = true;
          fontFile.extension = "ttf";
          fontFile.path = fontsAdditionalPath + iteratorFolder->path().filename().string();
          fontFile.title = iteratorFolder->path().filename().string();
          boost::replace_all(fontFile.title, ".ttf", "");
          this->fonts.push_back(fontFile);
          if (fontFile.path == Settings::Instance()->UIFontFile)
            Settings::Instance()->UIFontFileIndex = i;
          i += 1;
        }
      }
      catch (const std::exception & ex) {
        Settings::Instance()->funcDoLog(iteratorFolder->path().filename().string() + " " + ex.what());
      }
    }
  }
}

void FontsList::loadFontsWindows() {
  //| %WINDIR%\fonts

#ifdef WIN32
  std::string fontsFolder = "%WINDIR%\fonts";

  TCHAR szPath[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, szPath))) {
#  ifndef UNICODE
    fontsFolder = szPath;
#  else
    std::wstring folderLocalAppDataW = szPath;
    fontsFolder = std::string(folderLocalAppDataW.begin(), folderLocalAppDataW.end());
#  endif
    fontsFolder += "\\fonts";
    fs::path currentPath(fontsFolder);

    if (fs::is_directory(currentPath)) {
      fs::directory_iterator iteratorEnd;
      int i = 1;
      for (fs::directory_iterator iteratorFolder(currentPath); iteratorFolder != iteratorEnd; ++iteratorFolder) {
        try {
          std::string fileExtension = iteratorFolder->path().extension().string();
          fs::file_status fileStatus = iteratorFolder->status();
          if (fileExtension == ".ttf" && (fs::is_directory(fileStatus) || fs::is_regular_file(fileStatus))) {
            FBEntity fontFile;
            fontFile.isFile = true;
            fontFile.extension = "ttf";
            fontFile.path = fontsFolder + iteratorFolder->path().filename().string();
            fontFile.title = iteratorFolder->path().filename().string();
            boost::replace_all(fontFile.title, ".ttf", "");
            this->fonts.push_back(fontFile);
            if (fontFile.path == Settings::Instance()->UIFontFile)
              Settings::Instance()->UIFontFileIndex = i;
            i += 1;
          }
        } catch (const std::exception& ex) {
          Settings::Instance()->funcDoLog(iteratorFolder->path().filename().string() + " " + ex.what());
        }
      }
    }
  }
#endif
}

void FontsList::loadFontsNix() {
  //| /usr/share/fonts
  //| /usr/local/share/fonts
  //| ~/.fonts
}

