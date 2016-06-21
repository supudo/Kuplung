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
    void initBuffers(MeshModel meshModel);
    void render(glm::mat4 matrixMVP, glm::vec4 outlineColor);
    MeshModel meshModel;

    glm::mat4 matrixTransform;

    GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
    glm::vec3 size, center;

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
