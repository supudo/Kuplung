//
//  Strings.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 6/3/25.
//  Copyright © 2025 supudo.net. All rights reserved.
//

#ifndef Strings_h
#define Strings_h

#include <string>
#include <iostream>
#include <sstream>

namespace KuplungApp::Helpers {
  static inline std::string trim(const std::string& line) {
    const char* WhiteSpace = " \t\v\r\n";
    std::size_t start = line.find_first_not_of(WhiteSpace);
    std::size_t end = line.find_last_not_of(WhiteSpace);
    return start == end ? std::string() : line.substr(start, end - start + 1);
  }

  static inline std::string trimRight(const std::string& line) {
    const char* WhiteSpace = " \t\v\r\n";
    std::size_t start = 0;
    std::size_t end = line.find_last_not_of(WhiteSpace);
    return start == end ? std::string() : line.substr(0, end + 1);
  }

  static inline std::vector<std::string> splitString(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
      tokens.push_back(token);
    }
    return tokens;
  }

  static inline std::string joinElementsToString(const std::vector<std::string>& elements, char delimiter) {
    std::ostringstream s;
    for (const auto& i : elements) {
      if (&i != &elements[0]) {
        s << delimiter;
      }
      s << i;
    }
    return s.str();
  }
} // namespace KuplungApp::helpers

#endif /* Strings_h */
