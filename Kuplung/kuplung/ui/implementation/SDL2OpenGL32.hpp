//
//  SDL2OpenGL32.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SDL2OpenGL32_hpp
#define SDL2OpenGL32_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/utilities/gl/GLIncludes.h"

class SDL2OpenGL32 {
public:
    void init(SDL_Window *window);
    bool ImGui_Implementation_Init();
    void ImGui_Implementation_Shutdown();
    void ImGui_Implementation_NewFrame();
    bool ImGui_Implementation_ProcessEvent(SDL_Event* event);
    void ImGui_Implementation_InvalidateDeviceObjects();
    bool ImGui_Implementation_CreateDeviceObjects();
    void ImGui_Implementation_CreateFontsTexture();
    void ImGui_Implementation_RenderDrawLists();

private:
    SDL_Window *sdlWindow;
    double gui_Time = 0.0f;
    bool gui_MousePressed[3] = { false, false, false };
    float gui_MouseWheel = 0.0f;
    GLuint gui_FontTexture = 0;
    int gui_ShaderHandle = 0, gui_VertHandle = 0, gui_FragHandle = 0;
    int gui_AttribLocationTex = 0, gui_AttribLocationProjMtx = 0;
    int gui_AttribLocationPosition = 0, gui_AttribLocationUV = 0, gui_AttribLocationColor = 0;
    unsigned int gui_VboHandle = 0, gui_VaoHandle = 0, gui_ElementsHandle = 0;
};

#endif /* SDL2OpenGL32_hpp */
