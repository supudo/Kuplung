//
//  GUIOptions.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUIOptions_hpp
#define GUIOptions_hpp

#include "utilities/gui/imgui/imgui.h"
#include "utilities/gui/components/GUIStyle.hpp"
#include "utilities/settings/Settings.h"
#include "utilities/settings/FontsList.hpp"

class GUIOptions {
public:
    void init(std::function<void(std::string)> doLog);
    void showOptionsWindow(ImGuiStyle* ref,
                           GUIStyle *guiStyle,
                           FontsList *fontLister,
                           int& optionsFontSelected,
                           int& optionsFontSizeSelected,
                           bool* p_opened = NULL,
                           bool* needsFontChange = NULL);
    void loadFonts(FontsList *fontLister,
                   int& optionsFontSelected,
                   bool* needsFontChange = NULL);

private:
    std::function<void(std::string)> doLog;
    void logMessage(std::string message);
};

#endif /* GUIOptions_hpp */
