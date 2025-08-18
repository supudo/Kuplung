//
//  Files.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 6/3/25.
//  Copyright © 2025 supudo.net. All rights reserved.
//

#ifndef Files_h
#define Files_h

#include <string>

namespace KuplungApp::Helpers {
  static inline bool isHidden(const std::string& fileName) {
    return fileName == ".." || fileName == "." || fileName.compare(0, 1, ".");
  }

  static inline std::string getFilename(const std::string& file) {
    return file.substr(file.find_last_of("/\\") + 1);
  }
} // namespace KuplungApp::helpers

#endif /* Files_h */
