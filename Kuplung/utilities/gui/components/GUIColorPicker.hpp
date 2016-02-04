//
//  GUIColorPicker.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUIColorPicker_hpp
#define GUIColorPicker_hpp

#include "utilities/gui/ImGui/imgui.h"

class GUIColorPicker {
public:
    void show(const char* title, bool* p_opened, float* col, bool show_alpha);
    bool ColorPicker4(float* col, bool show_alpha);
    bool ColorPicker3(float col[3]);
};

#endif /* GUIColorPicker_hpp */
