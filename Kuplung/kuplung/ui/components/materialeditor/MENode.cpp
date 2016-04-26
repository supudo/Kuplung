//
//  MENode.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/materialeditor/MENode.hpp"
#include <math.h>
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"

// NB: You can use math functions/operators on ImVec2 if you #define IMGUI_DEFINE_MATH_OPERATORS and #include "imgui_internal.h"
// Here we only declare simple +/- operators so others don't leak into the demo code.
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
//static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

void MENode::init(int id, MaterialEditor_NodeType nodeType, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename, std::string textureImage) {
    this->ID = id;
    this->NodeType = nodeType;
    this->Name = name;
    this->Pos = pos;
    this->Value = value;
    this->Color = color;
    this->InputsCount = inputs_count;
    this->OutputsCount = outputs_count;
    this->TextureFilename = textureFilename;
    this->TextureImage = textureImage;
    this->IsExpanded = true;
}

void MENode::draw(ImVec2 node_rect_min, ImVec2 NODE_WINDOW_PADDING, bool showPreview, float scale) {
    ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
    ImGui::BeginGroup();
    ImGui::TextColored(ImColor(255, 0, 0), "%s", this->Name.c_str());
    ImGui::EndGroup();
}

ImVec2 MENode::GetInputSlotPos(int slot_no, float scale) const {
    return ImVec2(Pos.x * scale, Pos.y * scale + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1));
}

ImVec2 MENode::GetOutputSlotPos(int slot_no, float scale) const {
    return ImVec2(Pos.x * scale + Size.x, Pos.y * scale + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1));
}
