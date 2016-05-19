//
//  UVPoint.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/uveditor/UVPoint.hpp"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

UVPoint::UVPoint(int id, ImVec2 position, ImColor color, float radius) {
    this->ID = id;
    this->position = position;
    this->color = color;
    this->radius = radius;
    this->isDragging = false;
}

void UVPoint::draw(ImVec2 pointRect) {
    ImGui::SetCursorScreenPos(pointRect);
    ImGui::BeginGroup();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddCircleFilled(pointRect, this->radius, this->color);

    ImGui::EndGroup();
}
