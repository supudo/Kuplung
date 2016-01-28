//
//  GUIColorPicker.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUIColorPicker_hpp
#define GUIColorPicker_hpp

class GUIColorPicker {
public:
    bool ColorPicker4(float* col, bool show_alpha);
    bool ColorPicker3(float col[3]);
};

#endif /* GUIColorPicker_hpp */
