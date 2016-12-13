//
//  Shadertoy.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Shadertoy_hpp
#define Shadertoy_hpp

#include <string>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class Shadertoy {
public:
    ~Shadertoy();
    Shadertoy();
    bool initShaderProgram(std::string const& fragmentShaderSource);
    void initBuffers();
    void initTextures();
    void initFBO(const int windowWidth, const int windowHeight, GLuint* vboTexture);
    void render(const int mouseX, const int mouseY, const float seconds);
    void renderToTexture(const int mouseX, const int mouseY, const float seconds, GLuint* vboTexture);

    std::string iChannel0_Image, iChannel1_Image, iChannel2_Image, iChannel3_Image;
    std::string iChannel0_CubeImage, iChannel1_CubeImage, iChannel2_CubeImage, iChannel3_CubeImage;
    int textureWidth, textureHeight;

    std::unique_ptr<GLUtils> glUtils;

private:
    void addTexture(std::string const& textureImage, GLuint* vboTexture, const int textureID);
    float iChannelResolution0[2], iChannelResolution1[2], iChannelResolution2[2], iChannelResolution3[2];

    GLuint shaderProgram, shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices;
    GLint vs_InFBO, vs_ScreenResolution;

    GLint iResolution, iGlobalTime, iTimeDelta, iFrame, iFrameRate;
    GLint iChannelTime[4], iChannelResolution[4];
    GLint iMouse, iDate;
    GLuint iChannel0, iChannel1, iChannel2, iChannel3;

    void bindFBO();
    void unbindFBO(GLuint* vboTexture);
    GLuint tFBO, tRBO;
};

#endif /* Shadertoy_hpp */
