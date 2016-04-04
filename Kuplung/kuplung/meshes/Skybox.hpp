//
//  Skybox.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Skybox_hpp
#define Skybox_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class Skybox {
public:
    ~Skybox();
    void destroy();
    bool init();
    void render(glm::mat4 matrixView, float plane_close, float plane_far, float fov);

private:
    GLUtils *glUtils;
    GLuint shaderProgram, shaderVertex, shaderFragment, vboVertices, vboIndices, vboTexture;
    GLuint glVAO, glVS_MVPMatrix, glVS_VertexPosition;

    std::string readFile(const char *filePath);
};

#endif /* ModelFace_hpp */
