//
//  Controls.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/9/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Controls_hpp
#define Controls_hpp

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#ifdef _WIN32
#  undef main
#endif
#include "kuplung/settings/Settings.h"

namespace KuplungApp {
namespace Utilities {
namespace Input {

struct ControlPoint {
  int x, y;
};

class Controls {
public:
  Controls();
  ~Controls();
  void init(SDL_Window* sdlWindow);
  void processEvents(const SDL_Event* ev);

  bool gameIsRunning, keyPressed_ESC, keyPressed_DELETE;

  bool mouseButton_LEFT, mouseButton_MIDDLE, mouseButton_RIGHT;
  bool mouseGoLeft, mouseGoRight, mouseGoUp, mouseGoDown;

  bool keyPressed_LALT, keyPressed_LSHIFT, keyPressed_LCTRL;
  bool keyPressed_RALT, keyPressed_RSHIFT, keyPressed_RCTRL;
  bool keyPresset_TAB;

  int xrel, yrel;

  ControlPoint mouseWheel, mousePosition;

private:
  SDL_Window* sdlWindow;

  void handleInput(const SDL_Event* ev);
  void handleKeyDown(const SDL_Event* ev);
  void handleMouse(const SDL_Event* ev);
  void handleMouseWheel(const SDL_Event* ev);
  void handleMouseMotion(const SDL_Event* ev);
};

} // namespace Input
} // namespace Utilities
} // namespace KuplungApp

#endif /* Controls_hpp */
