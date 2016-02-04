//
//  GUILog.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUILog_hpp
#define GUILog_hpp

#include "utilities/gui/imgui/imgui.h"

class GUILog {
public:
    void init(int positionX, int positionY, int width, int height);
    void clear();
    void addToLog(const char* fmt, ...) IM_PRINTFARGS(2);
    void draw(const char* title, bool* p_opened = NULL);

    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets;
    bool ScrollToBottom;

private:
    int positionX, positionY, width, height;
};

#endif /* GUILog_hpp */
