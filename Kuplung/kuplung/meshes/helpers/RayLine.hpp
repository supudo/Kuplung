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
    void destroy();
    void init();
    bool initShaderProgram();
    void initBuffers(glm::vec3 vecFrom, glm::vec3 vecTo);
    void initProperties();
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera);

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboColors, vboIndices;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix, glUniformColor;

    std::vector<float> dataVertices, dataColors;
    std::vector<uint> dataIndices;

    std::string readFile(const char *filePath);
};

#endif /* RayLine_hpp */
