//
//  VertexSphere.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef VertexSphere_hpp
#define VertexSphere_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class VertexSphere {
public:
    VertexSphere();
    ~VertexSphere();
    bool initShaderProgram();
    void initBuffers(const MeshModel meshModel, const int circleSegments, const float radius);
    void render(const glm::mat4 matrixMVP, const glm::vec4 color);

    bool isSphere;
    bool showWireframes;
    int circleSegments;
    std::vector<glm::vec3> dataVertices;
    std::vector<glm::vec3> dataNormals;
    std::vector<GLuint> dataIndices;
    glm::mat4 matrixMVP;

private:
    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboIndices;

    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeVertexNormal, glUniformInnerLightDirection, glUniformColor;
};

#endif /* VertexSphere_hpp */
