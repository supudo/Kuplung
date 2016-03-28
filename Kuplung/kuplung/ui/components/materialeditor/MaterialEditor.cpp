//
//  MaterialEditor.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/materialeditor/MaterialEditor.hpp"
#include <math.h>
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/utilities/imgui/imgui_internal.h"

// Creating a node graph editor for ImGui
// Quick demo, not production code! This is more of a demo of how to use ImGui to create custom stuff.
// Better version by @daniel_collin here https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2
// See https://github.com/ocornut/imgui/issues/306
// v0.02
// Animated gif: https://cloud.githubusercontent.com/assets/8225057/9472357/c0263c04-4b4c-11e5-9fdf-2cd4f33f6582.gif

// NB: You can use math functions/operators on ImVec2 if you #define IMGUI_DEFINE_MATH_OPERATORS and #include "imgui_internal.h"
// Here we only declare simple +/- operators so others don't leak into the demo code.
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

// Really dumb data structure provided for the example.
// Note that we storing links are INDICES (not ID) to make example code shorter, obviously a bad idea for any general purpose code.

void MaterialEditor::init() {
    this->style_LinkColor = ImColor(200, 200, 100);
    this->style_LinkThickness = 3.0f;
    this->style_ShowImages = false;
    this->panelWidth_Nodes = 100.0f;
}

void MaterialEditor::draw(ModelFace *face, bool* p_opened) {
    ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Material Editor", p_opened, ImGuiWindowFlags_ShowBorders);

    if (!this->inited)
        this->initMaterialNodes(face);

    // Draw a list of nodes on the left side
    bool open_context_menu = false;
    int node_hovered_in_list = -1;
    int node_hovered_in_scene = -1;
    ImGui::BeginChild("node_list", ImVec2(this->panelWidth_Nodes, 0));
    ImGui::Text("Nodes");
    ImGui::Separator();
    for (size_t i=0; i<this->nodes.size(); i++) {
        MENode* node = (MENode*)this->nodes[i];
        ImGui::PushID(node->ID);
        if (ImGui::Selectable(node->Name.c_str(), node->ID == this->node_selected))
            this->node_selected = node->ID;
        if (ImGui::IsItemHovered()) {
            node_hovered_in_list = node->ID;
            open_context_menu |= ImGui::IsMouseClicked(1);
        }
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(89, 91, 94));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(119, 122, 124));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0));
    ImGui::Button("###splitterNodes", ImVec2(8.0f, -1));
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemActive())
        this->panelWidth_Nodes += ImGui::GetIO().MouseDelta.x;
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(4);

    ImGui::SameLine();

    ImGui::BeginGroup();

    const float NODE_SLOT_RADIUS = 6.0f;
    const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

    // Create our child canvas
    ImGui::Text("Hold middle mouse button to scroll (%.2f, %.2f)", this->scrolling.x, this->scrolling.y);
    ImGui::SameLine(ImGui::GetWindowWidth() - 220);
    ImGui::Checkbox("Show images", &this->style_ShowImages);
    ImGui::SameLine(ImGui::GetWindowWidth() - 100);
    ImGui::Checkbox("Show grid", &this->show_grid);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(60, 60, 70, 200));
    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove);
    ImGui::PushItemWidth(120.0f);

    ImVec2 offset = ImGui::GetCursorScreenPos() - this->scrolling;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSplit(2);

    // Display grid
    if (this->show_grid) {
        ImU32 GRID_COLOR = ImColor(200, 200, 200, 40);
        float GRID_SZ = 64.0f;
        ImVec2 win_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_sz = ImGui::GetWindowSize();
        for (float x = fmodf(offset.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
            draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
        for (float y = fmodf(offset.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
            draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
    }

    // Display links
    draw_list->ChannelsSetCurrent(0); // Background
    for (size_t i = 0; i < this->links.size(); i++) {
        MELink* link = this->links[i];
        ImVec2 p1 = offset + link->NodeOutput->GetOutputSlotPos(link->SlotOutput);
        ImVec2 p2 = offset + link->NodeInput->GetInputSlotPos(link->SlotInput);
        draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, this->style_LinkColor, this->style_LinkThickness);
    }

    bool isMouseDraggingForScrolling = ImGui::IsMouseDragging(2, 0.0f);
    const ImGuiIO io = ImGui::GetIO();
    const ImVec2 mouseScreenPos = io.MousePos;
    const bool cantDragAnything = isMouseDraggingForScrolling;
    bool isDraggingForLinks = !cantDragAnything && ImGui::IsMouseDragging(0, 0.0f);
    bool isDragNodeValid = this->dragNode.isValid();
    const ImVec2 link_cp(10, 0);

    // Display dragging links
    if (isDraggingForLinks && this->dragNode.node != NULL) {
        if (this->dragNode.inputSlotIndex != -1) {
            ImVec2 p1 = offset + this->dragNode.node->GetOutputSlotPos(this->dragNode.inputSlotIndex);
            const ImVec2& p2 = mouseScreenPos;
            draw_list->AddBezierCurve(p1, p1 + link_cp, p2 - link_cp, p2, this->style_LinkColor, this->style_LinkThickness, 0);
        }
        else if (this->dragNode.outputSlotIndex != -1) {
            const ImVec2& p1 = mouseScreenPos;
            ImVec2 p2 = offset + this->dragNode.node->GetInputSlotPos(this->dragNode.outputSlotIndex);
            draw_list->AddBezierCurve(p1, p1 + link_cp, p2 - link_cp, p2, this->style_LinkColor, this->style_LinkThickness, 0);
        }
    }

    // Display nodes
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1,1,1,0));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1,1,1,0));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1,1,1,0));
    for (size_t i=0; i<this->nodes.size(); i++) {
        MENode* node = (MENode*)this->nodes[i];
        ImGui::PushID(node->ID);
        ImVec2 node_rect_min = offset + node->Pos;

        // Display node contents first
        draw_list->ChannelsSetCurrent(1); // Foreground
        bool old_any_active = ImGui::IsAnyItemActive();
        node->draw(node_rect_min, NODE_WINDOW_PADDING, this->style_ShowImages);

        // Save the size of what we have emitted and whether any of the widgets are being used
        bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
        node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING + (node->ID == 0 ? ImVec2(0, 40) : ImVec2(0,0));
        ImVec2 node_rect_max = node_rect_min + node->Size;

        // Display node box
        draw_list->ChannelsSetCurrent(0); // Background
        ImGui::SetCursorScreenPos(node_rect_min);
        ImGui::InvisibleButton("node", node->Size);
        if (ImGui::IsItemHovered()) {
            node_hovered_in_scene = node->ID;
            open_context_menu |= ImGui::IsMouseClicked(1);
        }
        bool node_moving_active = ImGui::IsItemActive();
        if (node_widgets_active || node_moving_active)
            this->node_selected = node->ID;
        if (node_moving_active && ImGui::IsMouseDragging(0))
            node->Pos = node->Pos + ImGui::GetIO().MouseDelta;

        ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && this->node_selected == node->ID)) ? ImColor(75, 75, 75) : ImColor(60, 60, 60);
        draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
        draw_list->AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);
        ImVec2 connectorScreenPos;
        for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++) {
            connectorScreenPos = offset + node->GetInputSlotPos(slot_idx);
            draw_list->AddCircleFilled(connectorScreenPos, NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));
            if (std::abs(mouseScreenPos.x - connectorScreenPos.x) < NODE_SLOT_RADIUS && std::abs(mouseScreenPos.y - connectorScreenPos.y) < NODE_SLOT_RADIUS) {
                if (isDraggingForLinks && isDragNodeValid && this->dragNode.node != node) {
                    for (size_t link_idx = 0; link_idx < this->links.size(); link_idx++) {
                        MELink* link = this->links[link_idx];
                        if (link->NodeInput == this->dragNode.node)
                            this->links.erase(this->links.begin() + link_idx);
                    }
                    this->links.push_back(new MELink(this->dragNode.node, this->dragNode.inputSlotIndex, node, slot_idx));
                    isDraggingForLinks = false;
                }
                else if (isDraggingForLinks && !isDragNodeValid) {
                    this->dragNode.node = node;
                    this->dragNode.outputSlotIndex = slot_idx;
                    this->dragNode.inputSlotIndex = -1;
                    this->dragNode.pos = mouseScreenPos;
                }
            }
        }
        for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++) {
            connectorScreenPos = offset + node->GetOutputSlotPos(slot_idx);
            draw_list->AddCircleFilled(connectorScreenPos, NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));
            if (std::abs(mouseScreenPos.x - connectorScreenPos.x) < NODE_SLOT_RADIUS && std::abs(mouseScreenPos.y - connectorScreenPos.y) < NODE_SLOT_RADIUS) {
                if (isDraggingForLinks && isDragNodeValid && this->dragNode.node != node) {
                    for (size_t link_idx = 0; link_idx < this->links.size(); link_idx++) {
                        MELink* link = this->links[link_idx];
                        if (link->NodeOutput == this->dragNode.node)
                            this->links.erase(this->links.begin() + link_idx);
                    }
                    this->links.push_back(new MELink(node, slot_idx, this->dragNode.node, this->dragNode.outputSlotIndex));
                    isDraggingForLinks = false;
                }
                else if (isDraggingForLinks && !isDragNodeValid) {
                    this->dragNode.node = node;
                    this->dragNode.outputSlotIndex = -1;
                    this->dragNode.inputSlotIndex = slot_idx;
                    this->dragNode.pos = mouseScreenPos;
                }
            }
        }
//        if (!isDraggingForLinks && !this->dragNode.isValid()) {
//            this->dragNode.node = NULL;
//            this->dragNode.inputSlotIndex = this->dragNode.outputSlotIndex = -1;
//        }

        ImGui::PopID();
    }
    ImGui::PopStyleColor(3);
    draw_list->ChannelsMerge();

    if (!isDraggingForLinks)
        this->dragNode.node = NULL;

    // Open context menu
    if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1)) {
        this->node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
        open_context_menu = true;
    }
    if (open_context_menu) {
        ImGui::OpenPopup("context_menu");
        if (node_hovered_in_list != -1)
            this->node_selected = node_hovered_in_list;
        if (node_hovered_in_scene != -1)
            this->node_selected = node_hovered_in_scene;
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("context_menu")) {
        MENode* node = this->node_selected != -1 ? (MENode*)this->nodes[this->node_selected] : NULL;
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
        if (node) {
            ImGui::Text("Node '%s'", node->Name.c_str());
            ImGui::Separator();
            if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
            if (ImGui::MenuItem("Delete", NULL, false, false)) {}
            if (ImGui::MenuItem("Copy", NULL, false, false)) {}
        }
        else {
            if (ImGui::MenuItem("Add Color"))
                this->nodes.push_back(new MENode_Color((int)this->nodes.size() + 1, "Color", scene_pos, 0.5f, ImColor(100, 100, 200), 0, 1));
            else if (ImGui::MenuItem("Add Texture"))
                this->nodes.push_back(new MENode_Texture((int)this->nodes.size() + 1, "Texture", scene_pos, 0.5f, ImColor(100, 100, 200), 1, 1));
            //if (ImGui::MenuItem("Paste", NULL, false, false)) {}
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    // Scrolling
    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        this->scrolling = this->scrolling - ImGui::GetIO().MouseDelta;

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::EndGroup();

    ImGui::End();
}

void MaterialEditor::initMaterialNodes(ModelFace *face) {
    MENode_Combine* node0 = new MENode_Combine(0, "fragColor", ImVec2(270.0, 0), 1.0f, ImColor(0, 200, 100), 0, 0);
    this->nodes.push_back(node0);

    int materialNodesCounter = 1;
    int slotsCounter = 0;

    ImVec2 nodePosition = ImVec2(40, 50);
    if (face->oFace.faceMaterial.textures_ambient.image != "") {
        MENode_Texture* node = new MENode_Texture(materialNodesCounter, "Ambient Texture", nodePosition, 1.0f, ImColor(255, 100, 100), 0, 1, face->oFace.faceMaterial.textures_ambient.filename, face->oFace.faceMaterial.textures_ambient.image);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        nodePosition.y += (this->style_ShowImages ? 100 : 80);
    }
    else {
        float r = face->oFace.faceMaterial.ambient.r;
        float g = face->oFace.faceMaterial.ambient.g;
        float b = face->oFace.faceMaterial.ambient.b;
        MENode_Color* node = new MENode_Color(materialNodesCounter, "Ambient Color", nodePosition, 1.0f, ImColor(r, g, b), 0, 1);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        nodePosition.y += 80;
    }
    materialNodesCounter += 1;
    slotsCounter += 1;

    if (face->oFace.faceMaterial.textures_diffuse.image != "") {
        MENode_Texture* node = new MENode_Texture(materialNodesCounter, "Diffuse Texture", nodePosition, 1.0f, ImColor(255, 100, 100), 0, 1, face->oFace.faceMaterial.textures_diffuse.filename, face->oFace.faceMaterial.textures_diffuse.image);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        nodePosition.y += (this->style_ShowImages ? 180 : 80);
    }
    else {
        float r = face->oFace.faceMaterial.diffuse.r;
        float g = face->oFace.faceMaterial.diffuse.g;
        float b = face->oFace.faceMaterial.diffuse.b;
        MENode_Color* node = new MENode_Color(1, "Diffuse Color", nodePosition, 1.0f, ImColor(r, g, b), 0, 1);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        nodePosition.y += 80;
    }
    materialNodesCounter += 1;
    slotsCounter += 1;

    if (face->oFace.faceMaterial.textures_dissolve.image != "") {
        MENode_Texture* node = new MENode_Texture(materialNodesCounter, "Dissolve Texture", nodePosition, 1.0f, ImColor(255, 100, 100), 0, 1, face->oFace.faceMaterial.textures_dissolve.filename, face->oFace.faceMaterial.textures_dissolve.image);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        materialNodesCounter += 1;
        slotsCounter += 1;
        nodePosition.y += (this->style_ShowImages ? 180 : 80);
    }

    if (face->oFace.faceMaterial.textures_specular.image != "") {
        MENode_Texture* node = new MENode_Texture(materialNodesCounter, "Specular Texture", nodePosition, 1.0f, ImColor(255, 100, 100), 0, 1, face->oFace.faceMaterial.textures_specular.filename, face->oFace.faceMaterial.textures_specular.image);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        nodePosition.y += (this->style_ShowImages ? 180 : 80);
    }
    else {
        float r = face->oFace.faceMaterial.specular.r;
        float g = face->oFace.faceMaterial.specular.g;
        float b = face->oFace.faceMaterial.specular.b;
        MENode_Color* node = new MENode_Color(3, "Specular Color", nodePosition, 1.0f, ImColor(r, g, b), 0, 1);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        nodePosition.y += 80;
    }
    materialNodesCounter += 1;
    slotsCounter += 1;

    if (face->oFace.faceMaterial.textures_specularExp.image != "") {
        MENode_Texture* node = new MENode_Texture(materialNodesCounter, "SpecularExp Texture", nodePosition, 1.0f, ImColor(255, 100, 100), 0, 1, face->oFace.faceMaterial.textures_specularExp.filename, face->oFace.faceMaterial.textures_specularExp.image);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        materialNodesCounter += 1;
        slotsCounter += 1;
        nodePosition.y += (this->style_ShowImages ? 180 : 80);
    }

    if (face->oFace.faceMaterial.textures_bump.image != "") {
        MENode_Texture* node = new MENode_Texture(materialNodesCounter, "Bump Map", nodePosition, 1.0f, ImColor(255, 100, 100), 0, 1, face->oFace.faceMaterial.textures_bump.filename, face->oFace.faceMaterial.textures_bump.image);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        materialNodesCounter += 1;
        slotsCounter += 1;
        nodePosition.y += (this->style_ShowImages ? 180 : 80);
    }

    if (face->oFace.faceMaterial.textures_displacement.image != "") {
        MENode_Texture* node = new MENode_Texture(materialNodesCounter, "Displacement Map", nodePosition, 1.0f, ImColor(255, 100, 100), 0, 1, face->oFace.faceMaterial.textures_displacement.filename, face->oFace.faceMaterial.textures_displacement.image);
        this->nodes.push_back(node);
        this->links.push_back(new MELink(node, 0, node0, slotsCounter));
        materialNodesCounter += 1;
        slotsCounter += 1;
        nodePosition.y += (this->style_ShowImages ? 180 : 80);
    }

    MENode_Combine* zeroNode = (MENode_Combine*)this->nodes.at(0);
    zeroNode->Pos = ImVec2(300.0, nodePosition.y / 2);
    zeroNode->InputsCount = materialNodesCounter - 1;

    this->inited = true;
}
