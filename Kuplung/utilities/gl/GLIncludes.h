//
//  GLIncludes.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GLIncludes_h
#define GLIncludes_h

#include <SDL2/SDL.h>

#ifdef _WIN32
#include <GL/glew.h>
#else
//#include <OpenGL/gl3.h>
//#include <OpenGL/glext.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>

#endif /* GLIncludes_h */
