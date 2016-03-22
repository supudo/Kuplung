//
//  UIHelpers.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "UIHelpers.hpp"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"

UIHelpers::UIHelpers() {
    this->componentColorPicker = new ColorPicker();
}

void UIHelpers::addControlsXYZ(std::string property, bool showAnimate, bool doMinus, bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float min, float limit) {
    this->addControlsSlider(property + " X", 1, step, min, limit, showAnimate, animatedFlag, animatedValue, doMinus, isFrame);
    this->addControlsSlider(property + " Y", 2, step, min, limit, showAnimate, animatedFlag, animatedValue, doMinus, isFrame);
    this->addControlsSlider(property + " Z", 3, step, min, limit, showAnimate, animatedFlag, animatedValue, doMinus, isFrame);
}

void UIHelpers::addControlsSlider(std::string title, int idx, float step, float min, float limit, bool showAnimate, bool* animatedFlag, float* animatedValue, bool doMinus, bool* isFrame) {
    if (title != "")
        ImGui::Text("%s", title.c_str());
    if (showAnimate) {
        std::string c_id = "##00" + std::to_string(idx);
        if (ImGui::Checkbox(c_id.c_str(), animatedFlag))
            this->animateValue(isFrame, animatedFlag, animatedValue, step, limit, doMinus);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s", title.c_str());
        ImGui::SameLine();
    }
    std::string s_id = "##10" + std::to_string(idx);
    ImGui::SliderFloat(s_id.c_str(), *(&animatedValue), min, limit);
}

void UIHelpers::addControlsIntegerSlider(std::string title, int idx, int min, int limit, int* animatedValue) {
    if (title != "")
        ImGui::Text("%s", title.c_str());
    std::string s_id = "##10" + std::to_string(idx);
    ImGui::SliderInt(s_id.c_str(), *(&animatedValue), min, limit);
}

void UIHelpers::addControlsSliderSameLine(std::string title, int idx, float step, float min, float limit, bool showAnimate, bool* animatedFlag, float* animatedValue, bool doMinus, bool* isFrame) {
    if (showAnimate) {
        std::string c_id = "##00" + std::to_string(idx);
        if (ImGui::Checkbox(c_id.c_str(), animatedFlag))
            this->animateValue(&(*isFrame), &(*animatedFlag), animatedValue, step, limit, doMinus);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s", title.c_str());
        ImGui::SameLine();
    }
    std::string s_id = title + "##10" + std::to_string(idx);
    ImGui::SliderFloat(s_id.c_str(), *(&animatedValue), min, limit);
}

void UIHelpers::addControlColor3(std::string title, glm::vec3* vValue, bool* bValue) {
    std::string ce_id = "##101" + title;
    std::string icon_id = ICON_MD_COLORIZE + ce_id;
    ImGui::TextColored(ImVec4((*(&vValue))->r, (*(&vValue))->g, (*(&vValue))->b, 255.0), "%s", title.c_str());
    ImGui::ColorEdit3(ce_id.c_str(), (float*)&(*vValue));
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0, 0));
    if (ImGui::Button(icon_id.c_str(), ImVec2(0, 0)))
        *bValue = !*bValue;
    ImGui::PopStyleColor(4);
    if (*bValue)
        this->componentColorPicker->show(title.c_str(), &(*bValue), (float*)&(*vValue), true);
}

void UIHelpers::addControlColor4(std::string title, glm::vec4* vValue, bool* bValue) {
    std::string ce_id = "##101" + title;
    std::string icon_id = ICON_MD_COLORIZE + ce_id;
    ImGui::TextColored(ImVec4((*(&vValue))->r, (*(&vValue))->g, (*(&vValue))->b, (*(&vValue))->a), "%s", title.c_str());
    ImGui::ColorEdit4(ce_id.c_str(), (float*)&(*vValue), true);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0, 0));
    if (ImGui::Button(icon_id.c_str(), ImVec2(0, 0)))
        *bValue = !*bValue;
    ImGui::PopStyleColor(4);
    if (*bValue)
        this->componentColorPicker->show(title.c_str(), &(*bValue), (float*)&(*vValue), true);
}

void UIHelpers::animateValue(bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float limit, bool doMinus) {
    std::thread animThread(&UIHelpers::animateValueAsync, this, &(*isFrame), &(*animatedFlag), animatedValue, step, limit, doMinus);
    animThread.detach();
}

void UIHelpers::animateValueAsync(bool* isFrame, bool* animatedFlag, float* animatedValue, float step, float limit, bool doMinus) {
    while (*animatedFlag) {
        if (*isFrame) {
            float v = (*animatedValue);
            v += step;
            if (v > limit)
                v = (doMinus ? -1 * limit : 0);
            (*animatedValue) = v;
            (*isFrame) = false;
        }
    }
}
