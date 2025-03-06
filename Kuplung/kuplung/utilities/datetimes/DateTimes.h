//
//  DateTimes.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 6/3/25.
//  Copyright © 2025 supudo.net. All rights reserved.
//

#ifndef DateTimes_h
#define DateTimes_h

#include <filesystem>

static const std::string getDateToStringFormatted(const std::chrono::system_clock::duration& duration, const std::string& dateFormat) {
  const auto epoch = std::chrono::time_point<std::chrono::system_clock>();
  const auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(duration));
  const auto oldNow = epoch + dur;
  const auto t_c = std::chrono::system_clock::to_time_t(oldNow);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&t_c), dateFormat.c_str());
  return ss.str();
}

static const std::string getDateToString(const std::chrono::system_clock::duration& duration) {
  return getDateToStringFormatted(duration, "%Y-%m-%d %H:%M:%S");
}

#endif /* DateTimes_h */
