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
    void destroy();
    void init();
    bool initShaderProgram(std::string fragmentShaderSource);
    void initBuffers();
    void initTextures();
    void initFBO(int windowWidth, int windowHeight, GLuint* vboTexture);
    void render(int mouseX, int mouseY, float seconds);
    void renderToTexture(int mouseX, int mouseY, float seconds, GLuint* vboTexture);

    std::string iChannel0_Image, iChannel1_Image, iChannel2_Image, iChannel3_Image;
    std::string iChannel0_CubeImage, iChannel1_CubeImage, iChannel2_CubeImage, iChannel3_CubeImage;
    int textureWidth, textureHeight;

    std::unique_ptr<GLUtils> glUtils;

private:

    void addTexture(std::string textureImage, GLuint* vboTexture, int textureID);
    float iChannelResolution0[2], iChannelResolution1[2], iChannelResolution2[2], iChannelResolution3[2];

    GLuint shaderProgram, shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices;
    GLuint vs_InFBO, vs_ScreenResolution;

    GLint glAttributeVertexPosition;
    GLint iResolution, iGlobalTime, iTimeDelta, iFrame, iFrameRate;
    GLint iChannelTime[4], iChannelResolution[4];
    GLint iMouse, iDate;
    GLuint iChannel0, iChannel1, iChannel2, iChannel3;

    void bindFBO();
    void unbindFBO(GLuint* vboTexture);
    GLuint tFBO, tRBO, tRenderedTexture;
};

#endif /* Shadertoy_hpp */
