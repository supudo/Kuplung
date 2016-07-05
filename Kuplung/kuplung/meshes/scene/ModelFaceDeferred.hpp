//
//  ModelFaceDeferred.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceDeferred_hpp
#define ModelFaceDeferred_hpp

#include "kuplung/meshes/scene/ModelFace.hpp"
#include "kuplung/utilities/gl/GLUtils.hpp"

class ModelFaceDeferred: public ModelFace {
public:
    void destroy();
    bool initShader_GeometryPass();
    bool initShader_LightingPass();
    bool initShader_LightBox();

    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);

private:
    void renderGeometryPass();
    void renderLightingPass();
    void renderLightBox();
    void renderQuad();
    void renderCube();

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    const GLuint NR_LIGHTS = 32;

    // Model
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;

    // G-Buffer
    GLuint gBuffer, rboDepth;
    GLuint gPosition, gNormal, gAlbedoSpec;

    // Geometry Pass
    GLuint shaderProgram_GeometryPass;
    GLuint gl_GeometryPass_Position, gl_GeometryPass_Normal, gl_GeometryPass_TextureCoordinate;
    GLint gl_GeometryPass_ProjectionMatrix, gl_GeometryPass_ViewMatrix, gl_GeometryPass_ModelMatrix;
    GLuint gl_GeometryPass_TextureDiffuse, gl_GeometryPass_TextureSpecular;

    // Lighting Pass
    std::vector<ModelDeferred_LightSource> lightSources;
    GLuint shaderProgram_LightingPass;
    GLint gl_LightingPass_VPosition, gl_LightingPass_VTexCoords;
    GLint gl_LightingPass_Position, gl_LightingPass_Normal, gl_LightingPass_AlbedoSpec;
    GLint gl_LightingPass_ViewPosition, gl_LightingPass_DrawMode;

    // Light Box
    GLuint shaderProgram_LightBox;
    GLint gl_LightBox_ProjectionMatrix, gl_LightBox_ViewMatrix, gl_LightBox_ModelMatrix, gl_LightBox_LightColor;

    // Quad
    GLuint vaoQuad, vboQuad;

    // Cube
    GLuint vaoCube, vboCube;
};

#endif /* ModelFaceDeferred_hpp */
