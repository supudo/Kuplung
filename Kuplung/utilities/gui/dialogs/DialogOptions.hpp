//
//  DialogOptions.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef DialogOptions_hpp
#define DialogOptions_hpp

#include "utilities/gui/imgui/imgui.h"
#include "utilities/gui/dialogs/DialogStyle.hpp"
#include "utilities/settings/Settings.h"
#include "utilities/settings/FontsList.hpp"

class DialogOptions {
public:
    void init(std::function<void(std::string)> doLog);
    void showOptionsWindow(ImGuiStyle* ref, DialogStyle *wStyle, bool* p_opened = NULL, bool* needsFontChange = NULL);
    void loadFonts(bool* needsFontChange = NULL);

private:
    std::function<void(std::string)> doLog;
    void logMessage(std::string message);

    FontsList *fontLister;

    int optionsFontSelected, optionsFontSizeSelected;
};

#endif /* DialogOptions_hpp */
