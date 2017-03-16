//
//  KuplungIDE.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "KuplungIDE.hpp"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/settings/Settings.h"

void KuplungIDE::init() {
}

void KuplungIDE::draw(const char* title, bool* p_opened) {
    ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiSetCond_FirstUseEver);

    if (ImGui::Begin(title, p_opened, ImGuiWindowFlags_ShowBorders)) {
        ImGui::End();
    }
}
