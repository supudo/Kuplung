//
//  SceneRenderer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SceneRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/stb/stb_image.h"

SceneRenderer::~SceneRenderer() {
    this->destroy();
}

void SceneRenderer::destroy() {
}

void SceneRenderer::init() {
}

void SceneRenderer::renderImage(FBEntity file, std::vector<ModelFace*> *meshModelFaces, ObjectsManager *managerObjects) {
    int width = Settings::Instance()->SDL_Window_Width;
    int height = Settings::Instance()->SDL_Window_Height;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[SceneRenderer] Render Error: SDL could not initialize! SDL Error: %s\n", SDL_GetError()));
    else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");

        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        SDL_Window *gWindow = SDL_CreateWindow("Kuplung", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
        SDL_GLContext glContext;
        if (gWindow == NULL)
            Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[SceneRenderer] Render Error: Window could not be created! SDL Error: %s\n", SDL_GetError()));
        else {
            glContext = SDL_GL_CreateContext(gWindow);
            if (!glContext)
                Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[SceneRenderer] Render Error: Unable to create OpenGL context! SDL Error: %s\n", SDL_GetError()));
            else {
                if (SDL_GL_MakeCurrent(gWindow, glContext) < 0)
                    Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[SceneRenderer] Render Warning: Unable to set current context! SDL Error: %s\n", SDL_GetError()));
                else {
                    if (SDL_GL_SetSwapInterval(1) < 0)
                        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[SceneRenderer] Render Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError()));
                    else {
                        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
                        GLuint fbScreenshot = 0;
                        glGenFramebuffers(1, &fbScreenshot);
                        glBindFramebuffer(GL_FRAMEBUFFER, fbScreenshot);

                        // The texture we're going to render to
                        GLuint renderedTexture;
                        glGenTextures(1, &renderedTexture);

                        // "Bind" the newly created texture : all future texture functions will modify this texture
                        glBindTexture(GL_TEXTURE_2D, renderedTexture);

                        // Give an empty image to OpenGL ( the last "0" )
                        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

                        // Poor filtering. Needed !
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                        // The depth buffer
                        GLuint depthrenderbuffer;
                        glGenRenderbuffers(1, &depthrenderbuffer);
                        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
                        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
                        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

                        // Set "renderedTexture" as our colour attachement #0
                        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

                        // Set the list of draw buffers.
                        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
                        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

                        // Always check that our framebuffer is ok
                        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                            Settings::Instance()->funcDoLog("[SceneRenderer] Cannot create framebuffer!");

                        glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the
                        glClearColor(Settings::Instance()->guiClearColor.r, Settings::Instance()->guiClearColor.g, Settings::Instance()->guiClearColor.b, Settings::Instance()->guiClearColor.w);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                        // render models
                        for (int i=0; i<(int)(*meshModelFaces).size(); i++) {
                            ModelFace* mmf = (*meshModelFaces)[i];

                            glm::mat4 mtxModel = glm::mat4(1.0);

                            // reposition like the grid perspective
                            if (managerObjects->Setting_FixedGridWorld)
                                mtxModel *= managerObjects->grid->matrixModel;

                            // scale
                            mtxModel = glm::scale(mtxModel, glm::vec3(mmf->scaleX->point, mmf->scaleY->point, mmf->scaleZ->point));

                            // rotate
                            mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));
                            mtxModel = glm::rotate(mtxModel, glm::radians(mmf->rotateX->point), glm::vec3(1, 0, 0));
                            mtxModel = glm::rotate(mtxModel, glm::radians(mmf->rotateY->point), glm::vec3(0, 1, 0));
                            mtxModel = glm::rotate(mtxModel, glm::radians(mmf->rotateZ->point), glm::vec3(0, 0, 1));
                            mtxModel = glm::translate(mtxModel, glm::vec3(0, 0, 0));

                            // translate
                            mtxModel = glm::translate(mtxModel, glm::vec3(mmf->positionX->point, mmf->positionY->point, mmf->positionZ->point));

                            // general
                            mmf->setOptionsFOV(managerObjects->Setting_FOV);

                            // outlining
                            mmf->setOptionsSelected(0);
                            mmf->setOptionsOutlineColor(managerObjects->Setting_OutlineColor);
                            mmf->setOptionsOutlineThickness(managerObjects->Setting_OutlineThickness);

                            // lights
                            mmf->lightSources = managerObjects->lightSources;

                            // render
                            mmf->render(managerObjects->matrixProjection,
                                        managerObjects->camera->matrixCamera,
                                        mtxModel,
                                        managerObjects->camera->cameraPosition,
                                        managerObjects->grid,
                                        managerObjects->Setting_UIAmbientLight);
                        }

                        SDL_GL_SwapWindow(gWindow);

                        SDL_Surface *image = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);

                        glReadBuffer(GL_FRONT);
                        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

                        // flip vertically
                        int index;
                        void* temp_row;
                        int height_div_2;
                        temp_row = (void *)malloc(image->pitch);
                        if(NULL == temp_row)
                            Settings::Instance()->funcDoLog("Not enough memory for image inversion");
                        height_div_2 = (int) (image->h * .5);
                        for (index = 0; index < height_div_2; index++) {
                            memcpy((Uint8 *)temp_row,(Uint8 *)(image->pixels) + image->pitch * index, image->pitch);
                            memcpy((Uint8 *)(image->pixels) + image->pitch * index, (Uint8 *)(image->pixels) + image->pitch * (image->h - index - 1), image->pitch);
                            memcpy((Uint8 *)(image->pixels) + image->pitch * (image->h - index - 1), temp_row, image->pitch);
                        }
                        free(temp_row);

                        std::string f = file.path + "/" + file.title + ".bmp";

                        SDL_SaveBMP(image, f.c_str());
                        SDL_FreeSurface(image);

                        glDeleteFramebuffers(1, &fbScreenshot);
                        glDeleteTextures(1, &renderedTexture);
                        glDeleteRenderbuffers(1, &depthrenderbuffer);
                    }
                    SDL_GL_DeleteContext(glContext);
                }
            }
            SDL_DestroyWindow(gWindow);
        }
    }
}
