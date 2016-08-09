//
//  Controls.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/9/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Controls.hpp"

Controls::~Controls() {
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

    this->mouseWheel = {};
    this->mouseWheel.x = 0;
    this->mouseWheel.y = 0;
}

void Controls::processEvents(SDL_Event* ev) {
    switch (ev->type) {
        case SDL_QUIT:
            this->gameIsRunning = false;
            break;
        case SDL_KEYDOWN:
            this->handleInput(ev);
            break;
        case SDL_MOUSEBUTTONDOWN:
            this->handleInput(ev);
            break;
        case SDL_MOUSEBUTTONUP:
            this->handleInput(ev);
            break;
        case SDL_MOUSEMOTION:
            this->handleInput(ev);
            break;
        case SDL_MOUSEWHEEL:
            this->handleInput(ev);
            break;
    }
}

#pragma mark - Private

void Controls::handleInput(SDL_Event* ev) {
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

void Controls::handleKeyDown(SDL_Event* ev) {
    SDL_Keymod m = SDL_GetModState();
    this->keyPressed_ESC = false;
    this->keyPressed_DELETE = false;
    this->keyPressed_LALT = m & KMOD_LALT;
    this->keyPressed_LSHIFT = m & KMOD_LSHIFT;
    this->keyPressed_LCTRL = m & KMOD_LCTRL;
    this->keyPressed_RALT = m & KMOD_RALT;
    this->keyPressed_RSHIFT = m & KMOD_RSHIFT;
    this->keyPressed_RCTRL = m & KMOD_RCTRL;

    if (ev->type == SDL_KEYDOWN) {
        switch (ev->key.keysym.sym) {
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
        }
    }
}

void Controls::handleMouse(SDL_Event* ev) {
    if (ev->type == SDL_MOUSEBUTTONDOWN) {
        this->mouseButton_LEFT = false;
        this->mouseButton_MIDDLE = false;
        this->mouseButton_RIGHT = false;

        this->mouseButton_LEFT = ev->button.button == SDL_BUTTON_LEFT;
        this->mouseButton_MIDDLE = ev->button.button == SDL_BUTTON_MIDDLE;
        this->mouseButton_RIGHT = ev->button.button == SDL_BUTTON_RIGHT;
    }
    if (ev->type == SDL_MOUSEBUTTONUP) {
        this->mouseButton_LEFT = false;
        this->mouseButton_MIDDLE = false;
        this->mouseButton_RIGHT = false;
    }
}

void Controls::handleMouseWheel(SDL_Event* ev) {
    this->mouseWheel.x = 0;
    this->mouseWheel.y = 0;
    if (ev->type == SDL_MOUSEWHEEL) {
        this->mouseWheel.x = ev->wheel.x;
        this->mouseWheel.y = ev->wheel.y;
    }
    this->mousePosition.x = ev->motion.x;
    this->mousePosition.y = ev->motion.y;
}

void Controls::handleMouseMotion(SDL_Event* ev) {
    if (ev->type == SDL_MOUSEMOTION) {
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

