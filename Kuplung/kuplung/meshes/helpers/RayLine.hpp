//
//  RayLine.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RayLine_hpp
#define RayLine_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class RayLine {
public:
    ~RayLine();
    void init();
    bool initShaderProgram();
    void initBuffers(const glm::vec3 vecFrom, const glm::vec3 vecTo);
    void initProperties();
    void render(const glm::mat4 matrixProjection, const glm::mat4 matrixCamera);

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboColors, vboIndices;
    GLint glUniformMVPMatrix, glUniformColor;

    std::vector<float> dataVertices, dataColors;
    std::vector<uint> dataIndices;
};

#endif /* RayLine_hpp */
