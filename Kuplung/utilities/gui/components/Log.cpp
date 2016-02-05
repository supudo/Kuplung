//
//  GUILog.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "utilities/gui/components/Log.hpp"
#include "utilities/settings/Settings.h"

void Log::init(int positionX, int positionY, int width, int height) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
}

void Log::clear() {
    this->Buf.clear();
    this->LineOffsets.clear();
}

void Log::addToLog(const char* fmt, ...) {
    int old_size = this->Buf.size();
    va_list args;
    va_start(args, fmt);
    this->Buf.appendv(fmt, args);
    va_end(args);
    for (int new_size=this->Buf.size(); old_size<new_size; old_size++)
        if (this->Buf[old_size] == '\n')
            this->LineOffsets.push_back(old_size);
    this->ScrollToBottom = true;
}

void Log::draw(const char* title, bool* p_opened) {
    if (this->width > 0 && this->height > 0)
        ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiSetCond_FirstUseEver);
    else
        ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height), ImGuiSetCond_FirstUseEver);

    if (this->positionX > 0 && this->positionY > 0)
        ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiSetCond_FirstUseEver);

    ImGui::Begin(title, p_opened, ImGuiWindowFlags_ShowBorders);
    if (ImGui::Button("Clear"))
        this->clear();
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    this->Filter.Draw("Filter", -100.0f);
    ImGui::Separator();
    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
    if (copy)
        ImGui::LogToClipboard();

    if (this->Filter.IsActive()) {
        const char* buf_begin = this->Buf.begin();
        const char* line = buf_begin;
        for (int line_no = 0; line != NULL; line_no++) {
            const char* line_end = (line_no < this->LineOffsets.Size) ? buf_begin + this->LineOffsets[line_no] : NULL;
            if (this->Filter.PassFilter(line, line_end))
                ImGui::TextUnformatted(line, line_end);
            line = line_end && line_end[1] ? line_end + 1 : NULL;
        }
    }
    else
        ImGui::TextUnformatted(this->Buf.begin());

    if (this->ScrollToBottom)
        ImGui::SetScrollHere(1.0f);
    this->ScrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}
