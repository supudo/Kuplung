//
//  ImGui_Implementation_OpenGL32.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ImGui_Implementation_OpenGL32_hpp
#define ImGui_Implementation_OpenGL32_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/utilities/gl/GLIncludes.h"

class ImGui_Implementation_OpenGL32 {
public:
    bool Init(const char* glsl_version = "#version 150");
    void Shutdown();
    void NewFrame();
    bool ProcessEvent(SDL_Event* event);
    void DestroyDeviceObjects();
    bool CreateDeviceObjects();
    bool CreateFontsTexture();
    void RenderDrawData(ImDrawData* draw_data);
    void DestroyFontsTexture();

private:
    SDL_Window *sdlWindow;
    double gui_Time = 0.0f;
    bool gui_MousePressed[3] = { false, false, false };
    SDL_Cursor* gui_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };
    float gui_MouseWheel = 0.0f;
    GLuint gui_FontTexture = 0;
    int gui_ShaderHandle = 0, gui_VertHandle = 0, gui_FragHandle = 0;
    int gui_AttribLocationTex = 0, gui_AttribLocationProjMtx = 0;
    int gui_AttribLocationPosition = 0, gui_AttribLocationUV = 0, gui_AttribLocationColor = 0;
    unsigned int gui_VboHandle = 0, gui_VaoHandle = 0, gui_ElementsHandle = 0;
    char gui_GlslVersion[32] = "#version 150";
};

#endif /* ImGui_Implementation_OpenGL32_hpp */
