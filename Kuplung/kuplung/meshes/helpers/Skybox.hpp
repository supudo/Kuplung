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
#include "kuplung/objects/ObjectDefinitions.h"

class Skybox {
public:
    ~Skybox();
    void init(const int gridSize);
    bool initBuffers();
    void render(const glm::mat4 matrixView, const float plane_close, const float plane_far, const float fov);

    std::vector<Skybox_Item> skyboxItems;
    int Setting_Skybox_Item;

private:
    int gridSize;

    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram, shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboIndices, vboTexture;
    GLint glVS_MatrixView, glVS_MatrixProjection;
};

#endif /* ModelFace_hpp */
