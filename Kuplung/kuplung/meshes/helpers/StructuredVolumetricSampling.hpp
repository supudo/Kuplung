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
#include <glm/common.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class StructuredVolumetricSampling {
public:
    ~StructuredVolumetricSampling();
    void destroy();
    void init();
    bool initShaderProgram();
    void initBuffers();
    void render(glm::mat4 matrixMVP, int mouseX, int mouseY, float seconds);

private:
    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram, shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboTextureNoise;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix;
    GLint glFS_deltaRunningTime, glFS_noiseTextureSampler, glFS_screenResolution, glFS_mouseCoordinates;

private:
};

#endif /* StructuredVolumetricSampling_hpp */
