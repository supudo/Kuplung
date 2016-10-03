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
    void destroy();
    void init();
    bool initShaderProgram();
    void initBuffers();
    void initNoiseTexture();
    void render(int mouseX, int mouseY, float seconds);
    void renderToTexture(int mouseX, int mouseY, float seconds, int windowWidth, int windowHeight, GLuint* vboTexture);

private:
    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram, shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboTextureNoise;
    GLuint glVS_screenResolution;
    GLuint glAttributeVertexPosition, glFS_deltaRunningTime, glFS_noiseTextureSampler, glFS_screenResolution, glFS_mouseCoordinates;
};

#endif /* StructuredVolumetricSampling_hpp */
