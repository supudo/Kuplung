//
//  BoundingBox.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef BoundingBox_hpp
#define BoundingBox_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class BoundingBox {
public:
    BoundingBox();
    ~BoundingBox();
    void destroy();
    bool initShaderProgram();
    void initBuffers(objModelFace oFace);
    void render(glm::mat4 matrixMVP, glm::vec4 outlineColor);
    objModelFace oFace;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;
    glm::mat4 matrixTransform;

private:
    GLUtils *glUtils;

    std::vector<GLfloat> dataVertices;
    std::vector<GLuint> dataIndices;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboIndices;

    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glUniformColor;

    std::string readFile(const char *filePath);
};

#endif /* BoundingBox_hpp */