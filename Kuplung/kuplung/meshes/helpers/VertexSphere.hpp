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
    void destroy();
    bool initShaderProgram();
    void initBuffers(MeshModel meshModel, int circleSegments, float radius);
    void render(glm::mat4 matrixMVP, glm::vec4 color);

    glm::mat4 matrixTransform;

private:
    std::unique_ptr<GLUtils> glUtils;

    std::vector<glm::vec3> dataVertices;
    std::vector<GLuint> dataIndices;

    int circleSegments;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboIndices;

    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glUniformColor;
};

#endif /* VertexSphere_hpp */
