//
//  DialogGUIControls.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "DialogGUIControls.hpp"
#include "utilities/gui/Objects.h"
#include "utilities/gui/ImGui/imgui_internal.h"
#include "utilities/gui/components/IconsFontAwesome.h"
#include "utilities/gui/components/IconsMaterialDesign.h"
#include "utilities/settings/Settings.h"

void DialogGUIControls::init(std::function<void(std::string)> doLog) {
    this->doLog = doLog;
}

void DialogGUIControls::logMessage(std::string message) {
    this->doLog("[DialogGUIControls] " + message);
}

