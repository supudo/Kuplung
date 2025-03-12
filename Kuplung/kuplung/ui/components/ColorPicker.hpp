//
//  ColorPicker.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ColorPicker_hpp
#define ColorPicker_hpp

#include <imgui.h>

class ColorPicker {
public:
  void show(const char* title, bool* p_opened, float* col, const bool show_alpha);
  bool ColorPicker4(float* col, const bool show_alpha);
  bool ColorPicker3(float col[3]);
};

#endif /* ColorPicker_hpp */
