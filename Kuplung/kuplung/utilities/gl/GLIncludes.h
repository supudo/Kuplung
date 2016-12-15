//
//  GLIncludes.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GLIncludes_h
#define GLIncludes_h

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedefs"
#pragma clang diagnostic ignored "-Wunused-private-field"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wextern-c-compat"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-W#warnings"
#pragma clang diagnostic ignored "-Wpadded"

#include <SDL2/SDL.h>

#ifdef _WIN32
#include <GL/glew.h>
#include <SDL2/SDL_syswm.h>
#else
//#include <OpenGL/gl3.h>
//#include <OpenGL/glext.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>

#pragma clang diagnostic pop

#endif /* GLIncludes_h */
