//
//  RenderingForward.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingForward_hpp
#define RenderingForward_hpp

#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
//#include "kuplung/meshes/scene/ModelFaceForward.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/ui/Objects.h"

class RenderingForward {
public:
    explicit RenderingForward(ObjectsManager &managerObjects);
    ~RenderingForward();

    bool init();
    void render(std::vector<ModelFaceData*> meshModelFaces, const int selectedModel);

private:
    bool initShaderProgram();
    void renderModels(std::vector<ModelFaceData*> meshModelFaces, const int selectedModel);

    std::unique_ptr<GLUtils> glUtils;
    ObjectsManager &managerObjects;

    glm::mat4 matrixProjection, matrixCamera;
    glm::vec3 vecCameraPosition, uiAmbientLight;
    int lightingPass_DrawMode;

    // light
    unsigned int GLSL_LightSourceNumber_Directional, GLSL_LightSourceNumber_Point, GLSL_LightSourceNumber_Spot;
    std::vector<ModelFace_LightSource_Directional *> mfLights_Directional;
    std::vector<ModelFace_LightSource_Point *> mfLights_Point;
    std::vector<ModelFace_LightSource_Spot *> mfLights_Spot;

    // model objects
    GLuint shaderProgram;

    // variables
    GLint glVS_MVPMatrix, glFS_MMatrix, glVS_WorldMatrix, glVS_NormalMatrix, glFS_MVMatrix;

    // general
    GLint glGS_GeomDisplacementLocation, glFS_AlphaBlending, glFS_CameraPosition, glFS_CelShading;
    GLint glFS_OutlineColor, glVS_IsBorder, glFS_ScreenResX, glFS_ScreenResY, glFS_UIAmbient;
    GLint glTCS_UseCullFace, glTCS_UseTessellation, glTCS_TessellationSubdivision, gl_ModelViewSkin;
    GLint glFS_GammaCoeficient, glFS_showShadows, glFS_ShadowPass;

    // depth color
    GLint glFS_planeClose, glFS_planeFar, glFS_showDepthColor;

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
    // effects - tone mapping
    GLint glEffect_ToneMapping_ACESFilmRec2020;

    // view skin
    ModelFace_LightSource_Directional *solidLight;
    GLint glFS_solidSkin_materialColor;
};

#endif /* RenderingForward_hpp */
