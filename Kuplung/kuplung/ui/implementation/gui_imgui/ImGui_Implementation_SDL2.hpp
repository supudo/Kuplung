//
//  ImGui_Implementation_SDL2.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ImGui_Implementation_SDL2_hpp
#define ImGui_Implementation_SDL2_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/utilities/gl/GLIncludes.h"

class ImGui_Implementation_SDL2 {
public:
    const char* GetClipboardText(void*);
    bool Init(SDL_Window* window);
    bool InitForOpenGL(SDL_Window* window, void* sdl_gl_context);
    bool InitForVulkan(SDL_Window* window);
    void Shutdown();
    void UpdateMousePosAndButtons();
    void UpdateMouseCursor();
    void NewFrame(SDL_Window* window);
    bool ProcessEvent(SDL_Event* event);

private:
    SDL_Window *sdlWindow;
    Uint64 gui_Time = 0;
    bool gui_MousePressed[3] = { false, false, false };
    SDL_Cursor* gui_MouseCursors[ImGuiMouseCursor_Count_] = { 0 };
    char* gui_ClipboardTextData = NULL;
};

#endif /* ImGui_Implementation_SDL2_hpp */
