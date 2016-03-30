//
//  DialogOptions.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef DialogOptions_hpp
#define DialogOptions_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/ui/dialogs/DialogStyle.hpp"
#include "kuplung/settings/Settings.h"
#include "kuplung/settings/FontsList.hpp"

class DialogOptions {
public:
    void init();
    void showOptionsWindow(ImGuiStyle* ref, DialogStyle *wStyle, bool* p_opened = NULL, bool* needsFontChange = NULL);
    void loadFonts(bool* needsFontChange = NULL);

private:
    FontsList *fontLister;

    int optionsFontSelected, optionsFontSizeSelected;
};

#endif /* DialogOptions_hpp */
