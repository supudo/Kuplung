//
//  DialogOptions.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "DialogOptions.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/settings/Settings.h"

void DialogOptions::init() {
    this->fontLister = std::make_unique<FontsList>();
    this->fontLister->init();
    this->fontLister->getFonts();

    this->optionsFontSelected = Settings::Instance()->UIFontFileIndex;
    this->optionsFontSizeSelected = this->fontLister->getSelectedFontSize();
    this->optionsRendererType = Settings::Instance()->RendererType;
}

void DialogOptions::showOptionsWindow(ImGuiStyle* ref, DialogStyle *wStyle, bool* p_opened, bool* needsFontChange) {
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Options", p_opened, ImGuiWindowFlags_ShowBorders);

    if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        if (ImGui::Checkbox("Log Messages", &Settings::Instance()->logDebugInfo))
            Settings::Instance()->saveSettings();

        ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
        ImGui::BeginChild("RefreshRate", ImVec2(0.0f, 98.0f), true);
        ImGui::Text("Consumption Refresh Interval (in seconds, 0 - disabled)");
        ImGui::SliderInt("Memory", &Settings::Instance()->Consumption_Interval_Memory, 0, 100);
        ImGui::SliderInt("CPU", &Settings::Instance()->Consumption_Interval_CPU, 0, 100);
        if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth() * 0.65f, 0)))
            Settings::Instance()->saveSettings();
        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Rendering", NULL, true, false)) {
        ImGui::Indent();
        if (ImGui::Checkbox("Terrain Hieghtmap Image History", &Settings::Instance()->Terrain_HeightmapImageHistory))
            Settings::Instance()->saveSettings();

        const char* rendererItems[] = {"Simple", "Forward", "Forward with Shadow Mapping", "Deferred"};
        if (ImGui::Combo("Renderer", &this->optionsRendererType, rendererItems, IM_ARRAYSIZE(rendererItems))) {
            Settings::Instance()->RendererType = static_cast<InAppRendererType>(this->optionsRendererType);
            Settings::Instance()->saveSettings();
        }

        const char* parserItems[] = {"Kuplung Obj Parser 1.0", "Kuplung Obj Parser 2.0", "Assimp"};
        if (ImGui::Combo("ModelFace Parser", &Settings::Instance()->ModelFileParser, parserItems, IM_ARRAYSIZE(parserItems)))
            Settings::Instance()->saveSettings();
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Look & Feel", NULL, true, false)) {
        ImGui::Indent();
        ImGuiStyle& style = ImGui::GetStyle();

        const ImGuiStyle def;
        if (ImGui::Button("Default")) {
            style = ref ? *ref : def;
            style = wStyle->loadDefault();
            wStyle->save("-", "14.00", style);
            this->optionsFontSelected = 0;
            this->optionsFontSizeSelected = 1;
            Settings::Instance()->UIFontFileIndex = 0;
            Settings::Instance()->UIFontSize = 14.00;
            *needsFontChange = true;
        }
        if (ref) {
            ImGui::SameLine();
            if (ImGui::Button("Save")) {
                *ref = style;
                std::string font = "-";
                if (this->optionsFontSelected > 0 && this->fontLister->fontFileExists(this->fontLister->fonts[static_cast<size_t>(this->optionsFontSelected)].path))
                    font = this->fontLister->fonts[static_cast<size_t>(this->optionsFontSelected - 1)].path;
                std::string fontSize = std::string(this->fontLister->fontSizes[this->optionsFontSizeSelected]);
                Settings::Instance()->UIFontSize = std::stof(fontSize);
                wStyle->save(font, fontSize, style);
                *needsFontChange = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
            *p_opened = true;

        if (ImGui::TreeNode("Font")) {
            //const char* fonts[this->fontLister->fonts.size() + 1];
            //fonts[0] = " - < Default Font > ";
            //for (int i = 0, j = 1; i < (int)this->fontLister->fonts.size(); i++, j++) {
            //    fonts[j] = this->fontLister->fonts[i].title.c_str();
            //}
            std::string f1 = " - < Default Font > ";
            std::vector<const char*> fonts;
            fonts.push_back(f1.c_str());
            for (size_t i = 0, j = 1; i < this->fontLister->fonts.size(); i++, j++) {
                fonts.push_back(this->fontLister->fonts[i].title.c_str());
            }
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.60f);
            //ImGui::Combo("##001", &this->optionsFontSelected, fonts, IM_ARRAYSIZE(fonts));
            ImGui::Combo("##001", &this->optionsFontSelected, &fonts[0], int(this->fontLister->fonts.size() + 1));
            ImGui::PopItemWidth();

            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.30f);
            ImGui::Combo("##002", &this->optionsFontSizeSelected, this->fontLister->fontSizes, IM_ARRAYSIZE(this->fontLister->fontSizes));
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);

        if (ImGui::TreeNode("Rendering")) {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
            ImGui::PushItemWidth(100);
            ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
            if (style.CurveTessellationTol < 0.0f)
                style.CurveTessellationTol = 0.10f;
            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Sizes")) {
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Colors")) {
            static ImGuiColorEditMode edit_mode = ImGuiColorEditMode_RGB;
            ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditMode_RGB);
            ImGui::SameLine();
            ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditMode_HSV);
            ImGui::SameLine();
            ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditMode_HEX);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", 200);

            ImGui::BeginChild("#colors", ImVec2(0, 300), true);
            ImGui::PushItemWidth(-160);
            ImGui::ColorEditMode(edit_mode);
            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                const char* name = ImGui::GetStyleColName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
                if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0) {
                    ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : def.Colors[i];
                    if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
                }
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::TreePop();
        }

        ImGui::PopItemWidth();
        ImGui::Unindent();
    }

    ImGui::End();
}

void DialogOptions::loadFonts(bool* needsFontChange) {
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->Clear();

    if (this->optionsFontSelected == 0)
        Settings::Instance()->UIFontFile = "";
    else if (this->optionsFontSelected > 0 && this->fontLister->fontFileExists(this->fontLister->fonts[static_cast<size_t>(this->optionsFontSelected)].path))
        Settings::Instance()->UIFontFile = this->fontLister->fonts[static_cast<size_t>(this->optionsFontSelected)].path;

    if (Settings::Instance()->UIFontFile != "" && Settings::Instance()->UIFontFile != "-")
        io.Fonts->AddFontFromFileTTF(Settings::Instance()->UIFontFile.c_str(), Settings::Instance()->UIFontSize);
    else
        io.Fonts->AddFontDefault();

    // http://fortawesome.github.io/Font-Awesome/icons/
    std::string faFont = Settings::Instance()->appFolder() + "/fonts/fontawesome-webfont.ttf";
    static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig fa_config;
    fa_config.MergeMode = true;
    fa_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(faFont.c_str(), Settings::Instance()->UIFontSize, &fa_config, fa_ranges);

    // https://design.google.com/icons/
    std::string gmFont = Settings::Instance()->appFolder() + "/fonts/material-icons-regular.ttf";
    static const ImWchar gm_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
    ImFontConfig gm_config;
    gm_config.MergeMode = true;
    gm_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(gmFont.c_str(), Settings::Instance()->UIFontSize + 8.00f, &gm_config, gm_ranges);

    *needsFontChange = false;
}
