//
//  Log.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include "kuplung/utilities/imgui/imgui.h"

class Log {
public:
  void init(int positionX, int positionY, int width, int height);
  void clear();
  void addToLog(const char* fmt, ...) IM_FMTARGS(2);
  void draw(const char* title, bool* p_opened = NULL);

  ImGuiTextBuffer Buf;
  ImGuiTextFilter Filter;
  ImVector<int> LineOffsets;
  bool ScrollToBottom;

private:
  int positionX, positionY, width, height;
};

#endif /* Log_hpp */
