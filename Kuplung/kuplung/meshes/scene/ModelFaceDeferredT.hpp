//
//  ModelFaceDeferredT.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceDeferredT_hpp
#define ModelFaceDeferredT_hpp

#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/utilities/gl/GLUtils.hpp"

class ModelFaceDeferredT: public ModelFaceBase {
public:
    void init(MeshModel model, std::string const& assetsFolder);
    bool initShaderProgram();
    void initBuffers();
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);

private:
    GLuint shaderProgram_GeometryPass, shaderProgram_LightingPass, shaderProgram_LightBox;
    GLuint glVAO, vboVertices, vboNormals, vboTextureCoordinates, vboIndices;
    GLuint gl_TextureDiffuse, gl_TextureSpecular;

    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;

    const GLuint NR_LIGHTS = 32;
    std::vector<glm::vec3> objectPositions;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    GLuint quadVAO = 0;
    GLuint quadVBO;
    void renderQuad();

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;
    void renderCube();
};

#endif /* ModelFaceDeferredT_hpp */
