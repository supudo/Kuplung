//
//  GUIScreenshot.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUIScreenshot_hpp
#define GUIScreenshot_hpp

#define _CRT_SECURE_NO_WARNINGS
#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include <GLFW/glfw3.h>

class GUIScreenshot {
public:
    void ShowScreenshotsWindow(bool* open);
};

#endif /* GUIScreenshot_hpp */
