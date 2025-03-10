//
//  RenderingShadowMapping.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingShadowMapping_hpp
#define RenderingShadowMapping_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/objects/Objects.h"

class RenderingShadowMapping {
public:
  explicit RenderingShadowMapping(ObjectsManager& managerObjects);
  ~RenderingShadowMapping();

  bool init();
  void render(const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);

private:
  bool initShaderProgram();

  ObjectsManager& managerObjects;

  void renderShadows(const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);
  void renderModels(const bool& isShadowPass, const GLuint& sProgram, const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);
  void renderDepth();

  glm::mat4 matrixProjection, matrixCamera;
  glm::vec3 vecCameraPosition, uiAmbientLight;
  glm::mat4 matrixLightSpace;

  // shadows
  bool initShadows();
  bool initShadowsShader();
  bool initShadowsBuffers();
  GLuint shaderProgramShadows, shaderShadowsVertex, shaderShadowsFragment;
  GLuint fboDepthMap, vboDepthMap;
  GLint glShadow_ModelMatrix, glShadow_LightSpaceMatrix, glFS_ShadowPass, glFS_DebugShadowTexture;

  // shadows debug
  bool initShadowsDepth();
  bool initShadowsDepthShader();
  GLuint shaderProgramDepth, shaderDepthVertex, shaderDepthFragment;
  GLint glDepth_Plane_Close, glDepth_Plane_Far, glDepth_SamplerTexture;
  GLuint depthQuadVAO, depthQuadVBO;

  // light
  unsigned int GLSL_LightSourceNumber_Directional, GLSL_LightSourceNumber_Point, GLSL_LightSourceNumber_Spot;
  std::vector<std::unique_ptr<ModelFace_LightSource_Directional>> mfLights_Directional;
  std::vector<std::unique_ptr<ModelFace_LightSource_Point>> mfLights_Point;
  std::vector<std::unique_ptr<ModelFace_LightSource_Spot>> mfLights_Spot;

  // model objects
  GLuint shaderProgram;

  // variables
  GLint glVS_MVPMatrix, glFS_MMatrix, glVS_WorldMatrix, glVS_NormalMatrix, glFS_MVMatrix;

  // shadows
  GLint glVS_shadowModelMatrix, glVS_LightSpaceMatrix, glFS_showShadows, glFS_SamplerShadowMap;

  // general
  GLint glGS_GeomDisplacementLocation, glFS_AlphaBlending, glFS_CameraPosition, glFS_CelShading;
  GLint glFS_OutlineColor, glVS_IsBorder, glFS_ScreenResX, glFS_ScreenResY, glFS_UIAmbient;
  GLint glTCS_UseCullFace, glTCS_UseTessellation, glTCS_TessellationSubdivision, gl_ModelViewSkin;
  GLint glFS_GammaCoeficient;

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
  std::unique_ptr<ModelFace_LightSource_Directional> solidLight;
  GLint glFS_solidSkin_materialColor;
};

#endif /* RenderingShadowMapping_hpp */
