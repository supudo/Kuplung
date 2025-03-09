//
//  MENode_Color.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/materialeditor/MENode_Color.hpp"
#include <math.h>
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"

// NB: You can use math functions/operators on ImVec2 if you #define IMGUI_DEFINE_MATH_OPERATORS and #include "imgui_internal.h"
// Here we only declare simple +/- operators so others don't leak into the demo code.
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
//static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

MENode_Color::MENode_Color(int id, std::string const& name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string const& textureFilename, std::string const& textureImage) {
  MENode::init(id, MaterialEditor_NodeType_Color, name, pos, value, color, inputs_count, outputs_count, textureFilename, textureImage);
}

void MENode_Color::draw(ImVec2 node_rect_min, ImVec2 NODE_WINDOW_PADDING, bool showPreview, float scale) {
  ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
  ImGui::BeginGroup();

  ImGui::SetNextItemOpen(this->IsExpanded, ImGuiCond_Always);
  if (ImGui::TreeNode(this, "%s", "")) {
    ImGui::TreePop();
    this->IsExpanded = true;
  }
  else
    this->IsExpanded = false;
  ImGui::SameLine(0,0);
  ImGui::TextColored(ImColor(255, 0, 0), "%s", this->Name.c_str());

  if (this->IsExpanded) {
    ImGui::SliderFloat("##value", &this->Value, 0.0f, 1.0f, "Alpha %.2f");
    ImGui::ColorEdit3("##color", &this->Color.x);
  }

  ImGui::EndGroup();
}
