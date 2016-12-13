//
//  DialogSVS.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "DialogSVS.hpp"
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/utilities/stb/stb_image.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

void DialogSVS::render(bool* p_opened) {
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Structured Volumetric Sampling", p_opened, ImGuiWindowFlags_ShowBorders);

    this->windowWidth = ImGui::GetWindowWidth();
    this->windowHeight = ImGui::GetWindowHeight();
    this->textureWidth = int(this->windowWidth - this->viewPaddingHorizontal);
    this->textureHeight = int(this->windowHeight - this->viewPaddingVertical);

    this->structured_Volumetric_Sampling->renderToTexture(
            int(ImGui::GetIO().MousePos.x),
            int(ImGui::GetIO().MousePos.y),
            (SDL_GetTicks() / 1000.0f),
            &this->vboTexture
    );

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos() - this->scrolling;

    draw_list->ChannelsSetCurrent(0);
    ImVec2 bb_min = offset;
    ImVec2 bb_max = ImVec2(this->textureWidth, this->textureHeight) + offset;
    draw_list->AddImage(ImTextureID(intptr_t(this->vboTexture)), bb_min, bb_max);

    draw_list->ChannelsMerge();

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        this->scrolling = this->scrolling - ImGui::GetIO().MouseDelta;

    ImGui::End();
}

void DialogSVS::init() {
    this->viewPaddingHorizontal = 20.0f;
    this->viewPaddingVertical = 40.0f;
    this->textureWidth = int(this->windowWidth - this->viewPaddingHorizontal);
    this->textureHeight = int(this->windowHeight - this->viewPaddingVertical);

    this->structured_Volumetric_Sampling = std::make_unique<StructuredVolumetricSampling>();
    this->structured_Volumetric_Sampling->initShaderProgram();
    this->structured_Volumetric_Sampling->initBuffers();
    this->structured_Volumetric_Sampling->initFBO(
                Settings::Instance()->SDL_Window_Width,
                Settings::Instance()->SDL_Window_Height,
                &this->vboTexture);
}
