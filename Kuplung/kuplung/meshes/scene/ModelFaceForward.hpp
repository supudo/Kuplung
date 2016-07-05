//
//  ModelFaceForward.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceForward_hpp
#define ModelFaceForward_hpp

#include "kuplung/meshes/scene/ModelFace.hpp"

class ModelFaceForward: public ModelFace {
public:
    void init();
    void setModel(MeshModel meshModel);
    void initProperties();
    void initModelProperties();
    void renderModel();

    void destroy();
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);

private:
    void drawOutline();
    void drawOnly();

    bool reflectionInit();
    void renderReflectFBO();
    void renderMirrorSurface();

    // model objects
    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment, shaderGeometry, shaderTessControl, shaderTessEval;
    GLuint fboDefault, glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve;
    GLuint vboTextureBump, vboTextureDisplacement;

    // reflection objects
    GLuint shaderProgramReflection;
    GLuint shaderVertexReflection, shaderFragmentReflection;
    GLuint fboReflection, glVAOReflect;
    GLuint vboVerticesReflect, vboNormalsReflect, vboTextureCoordinatesReflect, vboIndicesReflect;
    GLuint reflectTexName, reflectWidth, reflectHeight;
    GLuint reflectModelViewUniformIdx, reflectProjectionUniformIdx, reflectNormalMatrixUniformIdx;

    // variables
    GLint glVS_MVPMatrix, glFS_MMatrix, glVS_WorldMatrix, glVS_NormalMatrix, glFS_MVMatrix;
    GLuint glVS_VertexPosition, glFS_TextureCoord, glVS_VertexNormal, glVS_Tangent, glVS_Bitangent;

    // general
    GLint glGS_GeomDisplacementLocation, glFS_AlphaBlending, glFS_CameraPosition, glFS_CelShading;
    GLint glFS_OutlineColor, glVS_IsBorder, glFS_ScreenResX, glFS_ScreenResY, glFS_UIAmbient;
    GLint glTCS_UseCullFace, glTCS_UseTessellation, glTCS_TessellationSubdivision, gl_ModelViewSkin;

    // material
    GLint glMaterial_Ambient, glMaterial_Diffuse, glMaterial_Specular, glMaterial_SpecularExp;
    GLint glMaterial_Emission, glMaterial_Refraction, glMaterial_IlluminationModel, glMaterial_HeightScale;
    GLint glMaterial_SamplerAmbient, glMaterial_SamplerDiffuse, glMaterial_SamplerSpecular;
    GLint glMaterial_SamplerSpecularExp, glMaterial_SamplerDissolve, glMaterial_SamplerBump, glMaterial_SamplerDisplacement;
    GLint glMaterial_HasTextureAmbient, glMaterial_HasTextureDiffuse, glMaterial_HasTextureSpecular;
    GLint glMaterial_HasTextureSpecularExp, glMaterial_HasTextureDissolve, glMaterial_HasTextureBump, glMaterial_HasTextureDisplacement;
    GLint glMaterial_ParallaxMapping;

    // effects - gaussian blur
    GLint glEffect_GB_W, glEffect_GB_Radius, glEffect_GB_Mode;
    // effects - bloom
    GLint glEffect_Bloom_doBloom, glEffect_Bloom_WeightA, glEffect_Bloom_WeightB, glEffect_Bloom_WeightC, glEffect_Bloom_WeightD, glEffect_Bloom_Vignette, glEffect_Bloom_VignetteAtt;
};

#endif /* ModelFaceForward_hpp */
