//
//  Screenshot.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Screenshot_hpp
#define Screenshot_hpp

#define _CRT_SECURE_NO_WARNINGS
#include "kuplung/utilities/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "kuplung/utilities/imgui/imgui_internal.h"

#include <GLFW/glfw3.h>

class Screenshot {
public:
  void ShowScreenshotsWindow(bool* open);
};

#endif /* Screenshot_hpp */
