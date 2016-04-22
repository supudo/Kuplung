//
//  AxisHelpers.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef AxisHelpers_hpp
#define AxisHelpers_hpp

#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class AxisHelpers {
public:
    void destroy();
    void init();
    void setModel(objModelFace oFace);
    void initProperties();
    bool initShaderProgram();
    void initBuffers();
    void render(glm::mat4 mtxProjection, glm::mat4 mtxCamera, float rotation, glm::vec3 position);
    objModelFace oFace;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glUniformColor;

    std::string readFile(const char *filePath);
};

#endif /* AxisHelpers_hpp */
