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

namespace Kuplung::Helpers {
  static inline bool isHidden(const std::string& fileName) {
    return fileName == ".." || fileName == "." || fileName.compare(0, 1, ".");
  }
} // namespace Kuplung::helpers

#endif /* Files_h */
