//
//  UIHelpers.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "UIHelpers.hpp"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/utilities/imgui/imgui.h"

UIHelpers::UIHelpers() : componentColorPicker(std::make_unique<ColorPicker>()) {}

void UIHelpers::addControlsXYZ(std::string const& property, const bool showAnimate, const bool doMinus, bool* isFrame, bool* animatedFlag, float* animatedValue, const float step, const float min, const float limit) const {
  this->addControlsSlider(property + " X", 1, step, min, limit, showAnimate, animatedFlag, animatedValue, doMinus, isFrame);
  this->addControlsSlider(property + " Y", 2, step, min, limit, showAnimate, animatedFlag, animatedValue, doMinus, isFrame);
  this->addControlsSlider(property + " Z", 3, step, min, limit, showAnimate, animatedFlag, animatedValue, doMinus, isFrame);
}

const bool UIHelpers::addControlsSlider(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const {
  if (!title.empty())
    ImGui::Text("%s", title.c_str());
  if (showAnimate) {
    std::string c_id = "##00" + std::to_string(idx);
    if (animatedFlag != NULL && ImGui::Checkbox(c_id.c_str(), animatedFlag))
      this->animateValue(isFrame, animatedFlag, animatedValue, step, limit, doMinus);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Animate %s", title.c_str());
    ImGui::SameLine();
  }
  std::string s_id = "##10" + std::to_string(idx);
  return ImGui::SliderFloat(s_id.c_str(), *(&animatedValue), min, limit);
}

const bool UIHelpers::addControlsIntegerSlider(std::string const& title, const int idx, const int min, const int limit, int* animatedValue) const {
  if (!title.empty())
    ImGui::Text("%s", title.c_str());
  std::string s_id = "##10" + std::to_string(idx);
  return ImGui::SliderInt(s_id.c_str(), *(&animatedValue), min, limit);
}

const bool UIHelpers::addControlsIntegerSliderSameLine(std::string const& title, const int idx, const int min, const int limit, int* animatedValue) const {
  std::string s_id = "##10" + std::to_string(idx);
  bool result = ImGui::SliderInt(s_id.c_str(), *(&animatedValue), min, limit);
  if (!title.empty()) {
    ImGui::SameLine();
    ImGui::Text("%s", title.c_str());
  }
  return result;
}

const bool UIHelpers::addControlsFloatSlider(std::string const& title, const int idx, const float min, const float limit, float* animatedValue) const {
  if (!title.empty())
    ImGui::Text("%s", title.c_str());
  std::string s_id = "##10" + std::to_string(idx);
  return ImGui::SliderFloat(s_id.c_str(), *(&animatedValue), min, limit);
}

const bool UIHelpers::addControlsFloatSliderSameLine(std::string const& title, const int idx, const float min, const float limit, float* animatedValue) const {
  std::string s_id = "##10" + std::to_string(idx);
  const bool result = ImGui::SliderFloat(s_id.c_str(), *(&animatedValue), min, limit, "%.03f");
  if (!title.empty()) {
    ImGui::SameLine();
    ImGui::Text("%s", title.c_str());
  }
  return result;
}

const bool UIHelpers::addControlsSliderSameLine(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const {
  if (showAnimate) {
    std::string c_id = "##00" + std::to_string(idx);
    if (ImGui::Checkbox(c_id.c_str(), animatedFlag))
      this->animateValue(isFrame, animatedFlag, animatedValue, step, limit, doMinus);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Animate %s", title.c_str());
    ImGui::SameLine();
  }
  std::string s_id = "##10" + std::to_string(idx);
  const bool r = ImGui::SliderFloat(s_id.c_str(), *(&animatedValue), min, limit);
  ImGui::SameLine();
  ImGui::Text("%s", title.c_str());
  return r;
}

void UIHelpers::addControlColor3(std::string const& title, glm::vec3* vValue, bool* bValue) const {
  std::string ce_id = "##101" + title;
  std::string icon_id = (char*)ICON_MD_COLORIZE + ce_id;
  ImGui::TextColored(ImVec4(vValue->r, vValue->g, vValue->b, 255.0), "%s", title.c_str());
  ImGui::ColorEdit3(ce_id.c_str(), (float*)vValue);
  ImGui::SameLine();
  ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  ImGui::PushStyleColor(ImGuiCol_Border, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  if (ImGui::Button(icon_id.c_str(), ImVec2(0, 0)))
    *bValue = !*bValue;
  ImGui::PopStyleColor(4);
  if (*bValue)
    this->componentColorPicker->show(title.c_str(), bValue, (float*)vValue, true);
}

void UIHelpers::addControlColor4(std::string const& title, glm::vec4* vValue, bool* bValue) const {
  std::string ce_id = "##101" + title;
  std::string icon_id = (char*)ICON_MD_COLORIZE + ce_id;
  ImGui::TextColored(ImVec4(vValue->r, vValue->g, vValue->b, vValue->a), "%s", title.c_str());
  ImGui::ColorEdit4(ce_id.c_str(), (float*)vValue, true);
  ImGui::SameLine();
  ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  ImGui::PushStyleColor(ImGuiCol_Border, static_cast<ImVec4>(ImColor(0, 0, 0, 0)));
  if (ImGui::Button(icon_id.c_str(), ImVec2(0, 0)))
    *bValue = !*bValue;
  ImGui::PopStyleColor(4);
  if (*bValue)
    this->componentColorPicker->show(title.c_str(), bValue, (float*)vValue, true);
}

void UIHelpers::animateValue(bool* isFrame, bool* animatedFlag, float* animatedValue, const float step, const float limit, const bool doMinus) const {
  std::thread animThread(&UIHelpers::animateValueAsync, this, isFrame, animatedFlag, animatedValue, step, limit, doMinus);
  animThread.detach();
}

void UIHelpers::animateValueAsync(bool* isFrame, const bool* animatedFlag, float* animatedValue, const float step, const float limit, const bool doMinus) const {
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

const bool UIHelpers::addControlsDrag(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const {
  if (!title.empty())
    ImGui::Text("%s", title.c_str());
  if (showAnimate) {
    std::string c_id = "##00" + std::to_string(idx);
    if (animatedFlag != NULL && ImGui::Checkbox(c_id.c_str(), animatedFlag))
      this->animateValue(isFrame, animatedFlag, animatedValue, step, limit, doMinus);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Animate %s", title.c_str());
    ImGui::SameLine();
  }
  std::string s_id = "##10" + std::to_string(idx);
  return ImGui::DragFloat(s_id.c_str(), *(&animatedValue), 0.001f, min, limit, "%.03f");
}

const bool UIHelpers::addControlsIntegerDrag(std::string const& title, const int idx, const int min, const int limit, int* animatedValue) const {
  if (!title.empty())
    ImGui::Text("%s", title.c_str());
  std::string s_id = "##10" + std::to_string(idx);
  return ImGui::DragInt(s_id.c_str(), *(&animatedValue), 0.001f, min, limit);
}

const bool UIHelpers::addControlsFloatDrag(std::string const& title, const int idx, const float min, float const limit, float* animatedValue) const {
  if (!title.empty())
    ImGui::Text("%s", title.c_str());
  std::string s_id = "##10" + std::to_string(idx);
  return ImGui::DragFloat(s_id.c_str(), *(&animatedValue), 0.001f, min, limit, "%.03f");
}

const bool UIHelpers::addControlsFloatDragSameLine(std::string const& title, const int idx, const float min, const float limit, float* animatedValue) const {
  std::string s_id = "##10" + std::to_string(idx);
  const bool result = ImGui::DragFloat(s_id.c_str(), *(&animatedValue), 0.001f, min, limit, "%.03f");
  if (!title.empty()) {
    ImGui::SameLine();
    ImGui::Text("%s", title.c_str());
  }
  return result;
}

const bool UIHelpers::addControlsDragSameLine(std::string const& title, const int idx, const float step, const float min, const float limit, const bool showAnimate, bool* animatedFlag, float* animatedValue, const bool doMinus, bool* isFrame) const {
  if (showAnimate) {
    std::string c_id = "##00" + std::to_string(idx);
    if (ImGui::Checkbox(c_id.c_str(), animatedFlag))
      this->animateValue(isFrame, animatedFlag, animatedValue, step, limit, doMinus);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Animate %s", title.c_str());
    ImGui::SameLine();
  }
  std::string s_id = title + "##10" + std::to_string(idx);
  return ImGui::DragFloat(s_id.c_str(), *(&animatedValue), 0.01f, min, limit, "%.03f");
}
