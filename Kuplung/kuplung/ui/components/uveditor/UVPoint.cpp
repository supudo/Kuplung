//
//  UVPoint.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/uveditor/UVPoint.hpp"

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
