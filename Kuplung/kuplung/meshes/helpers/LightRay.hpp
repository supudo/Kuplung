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
#include "kuplung/settings/Settings.h"

class LightRay {
public:
    LightRay();
    ~LightRay();
    bool initShaderProgram();
    void initBuffers(const glm::vec3& position, const glm::vec3& direction, const bool simple);
    void render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const glm::mat4& matrixModel);

private:
    short int axisSize;
    float x, y, z;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboIndices;
    GLint glUniformMVPMatrix;
};

#endif /* LightRay_hpp */
