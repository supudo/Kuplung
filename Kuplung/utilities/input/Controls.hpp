//
//  Controls.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/9/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Controls_hpp
#define Controls_hpp

#include <SDL2/SDL.h>
#include "utilities/settings/Settings.h"

struct ControlPoint {
    int x, y;
};

class Controls {
public:
    ~Controls();
    void init(std::function<void(std::string)> doLog, SDL_Window* sdlWindow);
    void processEvents(SDL_Event* ev);
    
    bool gameIsRunning;
    
    bool mouseButton_LEFT, mouseButton_MIDDLE, mouseButton_RIGHT;
    bool mouseGoLeft, mouseGoRight, mouseGoUp, mouseGoDown;
    
    bool keyPressed_LALT, keyPressed_LSHIFT, keyPressed_LCTRL;
    bool keyPressed_RALT, keyPressed_RSHIFT, keyPressed_RCTRL;
    
    int xrel, yrel;
    
    ControlPoint mouseWheel, mousePosition;
    
private:
    std::function<void(std::string)> doLogFunc;
    SDL_Window* sdlWindow;
    
    void handleInput(SDL_Event* ev);
    void handleKeyDown(SDL_Event* ev);
    void handleMouse(SDL_Event* ev);
    void handleMouseWheel(SDL_Event* ev);
    void handleMouseMotion(SDL_Event* ev);
};

#endif /* Controls_hpp */
