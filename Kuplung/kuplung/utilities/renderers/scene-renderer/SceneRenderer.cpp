//
//  SceneRenderer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SceneRenderer.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"

SceneRenderer::~SceneRenderer() {
    this->destroy();
}

void SceneRenderer::destroy() {
}

void SceneRenderer::init() {
}

void SceneRenderer::renderImage(FBEntity file) {
    int screenStats[4];
    glGetIntegerv(GL_VIEWPORT, screenStats);
    int width = screenStats[2];
    int height = screenStats[3];

    SDL_Surface *image = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    int index;
    void* temp_row;
    int height_div_2;
    temp_row = (void *)malloc(image->pitch);
    if(NULL == temp_row)
        SDL_SetError("Not enough memory for image inversion");
    height_div_2 = (int) (image->h * .5);
    for(index = 0; index < height_div_2; index++)
    {
        memcpy((Uint8 *)temp_row,(Uint8 *)(image->pixels) + image->pitch * index, image->pitch);
        memcpy((Uint8 *)(image->pixels) + image->pitch * index, (Uint8 *)(image->pixels) + image->pitch * (image->h - index - 1), image->pitch);
        memcpy((Uint8 *)(image->pixels) + image->pitch * (image->h - index - 1), temp_row, image->pitch);
    }
    free(temp_row);

    std::string f = file.path + "/" + file.title + ".bmp";

    SDL_SaveBMP(image, f.c_str());
    SDL_FreeSurface(image);
}
