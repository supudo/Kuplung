//
//  DialogShadertoy.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "DialogShadertoy.hpp"
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/utilities/stb/stb_image.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

void DialogShadertoy::render(bool* p_opened) {
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Shadertoy.com", p_opened, ImGuiWindowFlags_ShowBorders);

    this->windowWidth = ImGui::GetWindowWidth();
    this->windowHeight = ImGui::GetWindowHeight();
    this->textureWidth = this->windowWidth - this->viewPaddingHorizontal;
    this->textureHeight = this->windowHeight - this->viewPaddingVertical;

    this->engineShadertoy->renderToTexture(
            ImGui::GetIO().MousePos.x,
            ImGui::GetIO().MousePos.y,
            (SDL_GetTicks() / 1000.0f),
            &this->vboTexture
    );

// BEGIN preview
    ImGui::BeginChild("Preview", ImVec2(0, this->heightTopPanel), true);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos() - this->scrolling;

    draw_list->ChannelsSetCurrent(0);
    ImVec2 bb_min = offset;
    ImVec2 bb_max = ImVec2(this->textureWidth, this->textureHeight) + offset;
    draw_list->AddImage(ImTextureID(intptr_t(this->vboTexture)), bb_min, bb_max);

    draw_list->ChannelsMerge();

    ImGui::EndChild();
// END preview

// BEGIN preview delimiter
    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(89, 91, 94));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(119, 122, 124));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0));
    ImGui::Button("###splitterGUI", ImVec2(-1, 6.0f));
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemActive())
        this->heightTopPanel += ImGui::GetIO().MouseDelta.y;
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(3);
    else
        ImGui::GetIO().MouseDrawCursor = false;
// END preview delimiter

// BEGIN editor
    ImGui::BeginChild("Editor", ImVec2(0, 0), false);

// buttons
    if (ImGui::Button("COMPILE", ImVec2(ImGui::GetWindowWidth() * 0.85, this->buttonCompileHeight)))
        this->compileShader();
    ImGui::SameLine();
    if (ImGui::Button("Paste", ImVec2(ImGui::GetWindowWidth() * 0.14, this->buttonCompileHeight)))
        strcpy(this->shadertoyEditorText, this->paste().c_str());

// BEGIN textures
    ImGui::BeginChild("Textures", ImVec2(this->widthTexturesPanel, 0), false);

//    const char* availableTextureImages[] = {
//        "tex00.jpg", "tex01.jpg", "tex02.jpg", "tex03.jpg", "tex04.jpg", "tex05.jpg", "tex06.jpg", "tex07.jpg", "tex08.jpg", "tex09.jpg", "tex10.jpg",
//        "tex11.jpg", "tex12.jpg", "tex13.jpg", "tex14.jpg", "tex15.jpg", "tex16.jpg", "tex17.jpg", "tex18.jpg", "tex19.jpg", "tex20.jpg"
//    };

//    ImGui::PushItemWidth(100.0f);
//    ImGui::Text("Texture Image[0]");
//    ImGui::Combo("##textImage0", &this->texImage0, availableTextureImages, IM_ARRAYSIZE(availableTextureImages));
//    ImGui::Text("Texture Image[1]");
//    ImGui::Combo("##textImage1", &this->texImage1, availableTextureImages, IM_ARRAYSIZE(availableTextureImages));
//    ImGui::Text("Texture Image[2]");
//    ImGui::Combo("##textImage2", &this->texImage2, availableTextureImages, IM_ARRAYSIZE(availableTextureImages));
//    ImGui::Text("Texture Image[3]");
//    ImGui::Combo("##textImage3", &this->texImage3, availableTextureImages, IM_ARRAYSIZE(availableTextureImages));
//    ImGui::PopItemWidth();

    std::vector<const char*> textureImages;
    for (int i=0; i<21; i++) {
        std::string t = "tex" + std::string((i < 10) ? "0" : "") + std::to_string(i) + ".jpg";
        textureImages.push_back(t.c_str());
    }

    ImGui::PushItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 100));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(1, 0, 0, 1));
    ImGui::Text("Texture #0");
    ImGui::ListBox("##texImage0", &this->texImage0, &textureImages[0], int(textureImages.size()), 3);
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Texture %i", this->texImage0);
        ImGui::EndTooltip();
    }
    ImGui::Separator();
    ImGui::Text("Texture #1");
    ImGui::ListBox("##texImage1", &this->texImage1, &textureImages[0], int(textureImages.size()), 3);
    ImGui::Separator();
    ImGui::Text("Texture #2");
    ImGui::ListBox("##texImage2", &this->texImage2, &textureImages[0], int(textureImages.size()), 3);
    ImGui::Separator();
    ImGui::Text("Texture #3");
    ImGui::ListBox("##texImage3", &this->texImage3, &textureImages[0], int(textureImages.size()), 3);
    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(2);

    ImGui::EndChild();

// BEGIN textures delimiter
    ImGui::SameLine();
    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(89, 91, 94));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(119, 122, 124));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0));
    ImGui::Button("###splitterGUI2", ImVec2(4.0f, -1));
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemActive())
        this->widthTexturesPanel += ImGui::GetIO().MouseDelta.x;
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(4);
    else
        ImGui::GetIO().MouseDrawCursor = false;
    ImGui::SameLine();
// END textures delimiter

// BEGIN IDE
    ImGui::BeginChild("IDE", ImVec2(0.0f, 0.0f), false);
    int lines = (ImGui::GetWindowHeight() - this->buttonCompileHeight - 4.0f) / ImGui::GetTextLineHeight();
    ImGui::InputTextMultiline("##source", this->shadertoyEditorText, IM_ARRAYSIZE(this->shadertoyEditorText), ImVec2(-1.0f, ImGui::GetTextLineHeight() * lines), ImGuiInputTextFlags_AllowTabInput);
    ImGui::EndChild();

// END editor
    ImGui::EndChild();

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        this->scrolling = this->scrolling - ImGui::GetIO().MouseDelta;

    ImGui::End();
}

void DialogShadertoy::init() {
    this->viewPaddingHorizontal = 20.0f;
    this->viewPaddingVertical = 40.0f;
    this->heightTopPanel = 200.0f;
    this->widthTexturesPanel = 140.0f;
    this->buttonCompileHeight = 44.0f;
    this->textureWidth = this->windowWidth - this->viewPaddingHorizontal;
    this->textureHeight = this->windowHeight - this->viewPaddingVertical;

    const char* funcMain = "void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
            "{\n"
            "   vec2 uv = fragCoord.xy / iResolution.xy;\n"
            "   fragColor = vec4(uv, 0.5 + 0.5 * sin(iGlobalTime), 1.0);\n"
            "}\n\0";
    strcpy(this->shadertoyEditorText, funcMain);

    this->engineShadertoy = std::make_unique<Shadertoy>();
    this->engineShadertoy->init();
    this->engineShadertoy->initShaderProgram(std::string(funcMain));
    this->engineShadertoy->initBuffers();
    this->engineShadertoy->initFBO(
                Settings::Instance()->SDL_Window_Width,
                Settings::Instance()->SDL_Window_Height,
                &this->vboTexture);
}

void DialogShadertoy::compileShader() {
    this->engineShadertoy->initShaderProgram(std::string(this->shadertoyEditorText));
    this->engineShadertoy->initBuffers();
    this->engineShadertoy->initFBO(
                Settings::Instance()->SDL_Window_Width,
                Settings::Instance()->SDL_Window_Height,
                &this->vboTexture);
}

std::string DialogShadertoy::exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe)
        return "ERROR";

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

std::string DialogShadertoy::paste() {
#ifdef _WIN32
    //TODO: Pate text from Windows clipboard
#else
    return this->exec("pbpaste");
#endif
}
