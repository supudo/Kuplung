//
//  RenderingDeferred.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingDeferred_hpp
#define RenderingDeferred_hpp

#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"

class RenderingDeferred {
public:
    ~RenderingDeferred();
    bool init();
    void render(std::vector<ModelFaceData*> meshModelFaces,
                glm::mat4 matrixProjection,
                glm::mat4 matrixCamera,
                glm::vec3 vecCameraPosition,
                WorldGrid *grid,
                glm::vec3 uiAmbientLight,
                int lightingPass_DrawMode);

private:
    bool initGeometryPass();
    bool initLighingPass();
    bool initLights();
    void initProps();
    void initGBuffer();
    void initModels(std::vector<ModelFaceBase*> meshModelFaces);

    GLUtils *glUtils;

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

#endif /* RenderingDeferred_hpp */
