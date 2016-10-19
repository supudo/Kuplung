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
    void init(int gridSize);
    bool initBuffers();
    void render(glm::mat4 matrixView, float plane_close, float plane_far, float fov);

    std::vector<Skybox_Item> skyboxItems;
    int Setting_Skybox_Item;

private:
    int gridSize;

    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram, shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboIndices, vboTexture;
    GLuint glVS_MatrixView, glVS_MatrixProjection, glVS_VertexPosition;
};

#endif /* ModelFace_hpp */
