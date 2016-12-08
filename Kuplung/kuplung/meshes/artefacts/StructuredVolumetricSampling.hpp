//
//  StructuredVolumetricSampling.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef StructuredVolumetricSampling_hpp
#define StructuredVolumetricSampling_hpp

#include <string>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class StructuredVolumetricSampling {
public:
    ~StructuredVolumetricSampling();
    void init();
    bool initShaderProgram();
    void initBuffers();
    void initNoiseTexture();
    void initFBO(const int windowWidth, const int windowHeight, GLuint* vboTexture);
    void render(const int mouseX, const int mouseY, const float seconds);
    void renderToTexture(const int mouseX, const int mouseY, const float seconds, GLuint* vboTexture);

private:
    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram, shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboTextureNoise;
    GLuint glVS_screenResolution;
    GLuint glAttributeVertexPosition, glFS_deltaRunningTime, glFS_noiseTextureSampler, glFS_screenResolution, glFS_mouseCoordinates;

    void bindFBO();
    void unbindFBO(GLuint* vboTexture);
    GLuint tFBO, tRBO, tRenderedTexture;
};

#endif /* StructuredVolumetricSampling_hpp */
