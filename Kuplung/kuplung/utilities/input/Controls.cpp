//
//  Controls.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/9/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Controls.hpp"

namespace KuplungApp {
namespace Utilities {
namespace Input {

Controls::~Controls() {}

Controls::Controls() {
  this->gameIsRunning = false;
  this->keyPressed_ESC = false;
  this->keyPressed_DELETE = false;

  this->mouseButton_LEFT = false;
  this->mouseButton_MIDDLE = false;
  this->mouseButton_RIGHT = false;
  this->mouseGoLeft = false;
  this->mouseGoRight = false;
  this->mouseGoUp = false;
  this->mouseGoDown = false;

  this->keyPressed_LALT = false;
  this->keyPressed_LSHIFT = false;
  this->keyPressed_LCTRL = false;
  this->keyPressed_RALT = false;
  this->keyPressed_RSHIFT = false;
  this->keyPressed_RCTRL = false;
  this->keyPresset_TAB = false;

  this->xrel = 0;
  this->yrel = 0;
}

void Controls::init(SDL_Window* sdlWindow) {
  this->sdlWindow = sdlWindow;

  this->gameIsRunning = true;
  this->keyPressed_ESC = false;
  this->keyPressed_DELETE = false;

  this->mouseButton_LEFT = false;
  this->mouseButton_MIDDLE = false;
  this->mouseButton_RIGHT = false;

  this->keyPressed_LALT = false;
  this->keyPressed_LSHIFT = false;
  this->keyPressed_LCTRL = false;
  this->keyPressed_RALT = false;
  this->keyPressed_RSHIFT = false;
  this->keyPressed_RCTRL = false;

  this->keyPresset_TAB = false;

  this->mouseWheel = {};
  this->mouseWheel.x = 0;
  this->mouseWheel.y = 0;
}

void Controls::processEvents(const SDL_Event* ev) {
  switch (ev->type) {
  case SDL_EVENT_QUIT:
    this->gameIsRunning = false;
    break;
  case SDL_EVENT_KEY_DOWN:
    this->handleInput(ev);
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    this->handleInput(ev);
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    this->handleInput(ev);
    break;
  case SDL_EVENT_MOUSE_MOTION:
    this->handleInput(ev);
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    this->handleInput(ev);
    break;
  }
}

void Controls::handleInput(const SDL_Event* ev) {
  this->mouseWheel = {};
  this->mouseWheel.x = 0;
  this->mouseWheel.y = 0;
  this->mousePosition.x = 0;
  this->mousePosition.y = 0;

  this->handleKeyDown(ev);
  this->handleMouse(ev);
  this->handleMouseWheel(ev);
  this->handleMouseMotion(ev);
}

void Controls::handleKeyDown(const SDL_Event* ev) {
  SDL_Keymod m = SDL_GetModState();
  this->keyPressed_ESC = false;
  this->keyPressed_DELETE = false;
  this->keyPressed_LALT = m & SDL_KMOD_LALT;
  this->keyPressed_LSHIFT = m & SDL_KMOD_LSHIFT;
  this->keyPressed_LCTRL = m & SDL_KMOD_LCTRL;
  this->keyPressed_RALT = m & SDL_KMOD_RALT;
  this->keyPressed_RSHIFT = m & SDL_KMOD_RSHIFT;
  this->keyPressed_RCTRL = m & SDL_KMOD_RCTRL;

  if (ev->type == SDL_EVENT_KEY_DOWN) {
    switch (ev->key.key) {
      case SDLK_ESCAPE: {
        this->keyPressed_ESC = true;
        this->keyPressed_DELETE = false;
        this->mouseButton_LEFT = false;
        this->mouseButton_MIDDLE = false;
        this->mouseButton_RIGHT = false;
        this->mouseGoLeft = false;
        this->mouseGoRight = false;
        this->mouseGoUp = false;
        this->mouseGoDown = false;
        break;
      }
      case SDLK_DELETE: {
        this->keyPressed_ESC = false;
        this->keyPressed_DELETE = true;
        this->mouseButton_LEFT = false;
        this->mouseButton_MIDDLE = false;
        this->mouseButton_RIGHT = false;
        this->mouseGoLeft = false;
        this->mouseGoRight = false;
        this->mouseGoUp = false;
        this->mouseGoDown = false;
        break;
      }
      case SDLK_TAB: {
        this->keyPressed_ESC = false;
        this->keyPressed_DELETE = false;
        this->keyPresset_TAB = !this->keyPresset_TAB;
        this->mouseButton_LEFT = false;
        this->mouseButton_MIDDLE = false;
        this->mouseButton_RIGHT = false;
        this->mouseGoLeft = false;
        this->mouseGoRight = false;
        this->mouseGoUp = false;
        this->mouseGoDown = false;
        break;
      }
    }
  }
}

void Controls::handleMouse(const SDL_Event* ev) {
  if (ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    this->mouseButton_LEFT = false;
    this->mouseButton_MIDDLE = false;
    this->mouseButton_RIGHT = false;

    this->mouseButton_LEFT = ev->button.button == SDL_BUTTON_LEFT;
    this->mouseButton_MIDDLE = ev->button.button == SDL_BUTTON_MIDDLE;
    this->mouseButton_RIGHT = ev->button.button == SDL_BUTTON_RIGHT;
  }
  if (ev->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    this->mouseButton_LEFT = false;
    this->mouseButton_MIDDLE = false;
    this->mouseButton_RIGHT = false;
  }
}

void Controls::handleMouseWheel(const SDL_Event* ev) {
  this->mouseWheel.x = 0;
  this->mouseWheel.y = 0;
  if (ev->type == SDL_EVENT_MOUSE_WHEEL) {
    this->mouseWheel.x = ev->wheel.x;
    this->mouseWheel.y = ev->wheel.y;
  }
  this->mousePosition.x = ev->motion.x;
  this->mousePosition.y = ev->motion.y;
}

void Controls::handleMouseMotion(const SDL_Event* ev) {
  if (ev->type == SDL_EVENT_MOUSE_MOTION) {
    this->mouseGoLeft = false;
    this->mouseGoRight = false;
    this->mouseGoUp = false;
    this->mouseGoDown = false;

    this->xrel = ev->motion.xrel;
    this->yrel = ev->motion.yrel;

    if (ev->motion.xrel < 0)
      this->mouseGoLeft = true;
    else if (ev->motion.xrel > 0)
      this->mouseGoRight = true;
    else if (ev->motion.yrel < 0)
      this->mouseGoUp = true;
    else if (ev->motion.yrel > 0)
      this->mouseGoDown = true;
  }
}

} // namespace Input
} // namespace Utilities
} // namespace KuplungApp
