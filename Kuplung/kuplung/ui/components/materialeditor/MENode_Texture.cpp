//
//  MENode_Texture.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/materialeditor/MENode_Texture.hpp"
#include <math.h>
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"

// NB: You can use math functions/operators on ImVec2 if you #define IMGUI_DEFINE_MATH_OPERATORS and #include "imgui_internal.h"
// Here we only declare simple +/- operators so others don't leak into the demo code.
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

MENode_Texture::MENode_Texture(int id, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename, std::string textureImage) {
    MENode::init(id, MaterialEditor_NodeType_Image, name, pos, value, color, inputs_count, outputs_count, textureFilename, textureImage);
    this->showTextureWindow = false;
    this->loadTexture = false;
    strcpy(this->filePath, this->TextureImage.c_str());
}

void MENode_Texture::draw(ImVec2 node_rect_min, ImVec2 NODE_WINDOW_PADDING) {
    ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
    ImGui::BeginGroup();

    ImGui::SetNextTreeNodeOpened(this->IsExpanded, ImGuiSetCond_Always);
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

        std::string btnLabel = ICON_FA_EYE;
        if (ImGui::Button(btnLabel.c_str())) {
            this->showTextureWindow = !this->showTextureWindow;
            this->loadTexture = true;
        }
        ImGui::SameLine();
        ImGui::InputText("", this->filePath, sizeof(this->filePath), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        float bw = ImGui::CalcTextSize("...").x + 10;
        ImGui::PushItemWidth(bw);
        if (ImGui::Button("...")) {
        }
        ImGui::PopItemWidth();
    }

    ImGui::EndGroup();
}
