//
//  LightRay.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef LightRay_hpp
#define LightRay_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class LightRay {
public:
    ~LightRay();
    LightRay();
    void destroy();
    bool initShaderProgram();
    void initBuffers(glm::vec3 position, glm::vec3 direction, bool simple);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);

private:
    int axisSize;
    float x, y, z;

    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboIndices;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix;
};

#endif /* LightRay_hpp */
