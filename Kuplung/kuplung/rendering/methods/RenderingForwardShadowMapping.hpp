//
//  RenderingForwardShadowMapping.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingForwardShadowMapping_hpp
#define RenderingForwardShadowMapping_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/objects/Objects.h"

class RenderingForwardShadowMapping {
public:
  explicit RenderingForwardShadowMapping(ObjectsManager &managerObjects);
  ~RenderingForwardShadowMapping();

  bool init();
  void render(const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);

private:
  bool initShaderProgram();

  ObjectsManager &managerObjects;

  void renderShadows(const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);
  void renderModels(const bool& isShadowPass, const GLuint& sProgram, const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);
  void renderDepth();

  glm::mat4 matrixProjection;
  glm::mat4 matrixCamera;
  glm::vec3 vecCameraPosition;
  glm::vec3 uiAmbientLight;
  glm::mat4 matrixLightSpace;

  // shadows
  bool initShadows();
  bool initShadowsShader();
  bool initShadowsBuffers();
  GLuint shaderProgramShadows;
  GLuint shaderShadowsVertex;
  GLuint shaderShadowsFragment;
  GLuint fboDepthMap;
  GLuint vboDepthMap;
  GLint glShadow_ModelMatrix;
  GLint glShadow_LightSpaceMatrix;
  GLint glFS_ShadowPass;
  GLint glFS_DebugShadowTexture;

  // shadows debug
  bool initShadowsDepth();
  bool initShadowsDepthShader();
  GLuint shaderProgramDepth;
  GLuint shaderDepthVertex;
  GLuint shaderDepthFragment;
  GLint glDepth_Plane_Close;
  GLint glDepth_Plane_Far;
  GLint glDepth_SamplerTexture;
  GLuint depthQuadVAO;
  GLuint depthQuadVBO;

  // light
  unsigned int GLSL_LightSourceNumber_Directional;
  unsigned int GLSL_LightSourceNumber_Point;
  unsigned int GLSL_LightSourceNumber_Spot;
	std::vector<std::unique_ptr<ModelFace_LightSource_Directional>> mfLights_Directional;
	std::vector<std::unique_ptr<ModelFace_LightSource_Point>> mfLights_Point;
	std::vector<std::unique_ptr<ModelFace_LightSource_Spot>> mfLights_Spot;

  // model objects
  GLuint shaderProgram;

  // variables
  GLint glVS_MVPMatrix;
  GLint glFS_MMatrix;
  GLint glVS_WorldMatrix;
  GLint glVS_NormalMatrix;
  GLint glFS_MVMatrix;

  // shadows
  GLint glVS_shadowModelMatrix;
  GLint glVS_LightSpaceMatrix;
  GLint glFS_showShadows;
  GLint glFS_SamplerShadowMap;

  // general
  GLint glGS_GeomDisplacementLocation;
  GLint glFS_AlphaBlending;
  GLint glFS_CameraPosition;
  GLint glFS_CelShading;

  GLint glFS_OutlineColor;
  GLint glVS_IsBorder;
  GLint glFS_ScreenResX;
  GLint glFS_ScreenResY;
  GLint glFS_UIAmbient;

  GLint glTCS_UseCullFace;
  GLint glTCS_UseTessellation;
  GLint glTCS_TessellationSubdivision;
  GLint gl_ModelViewSkin;

  GLint glFS_GammaCoeficient;

  // depth color
  GLint glFS_planeClose;
  GLint glFS_planeFar;
  GLint glFS_showDepthColor;

  // material
  GLint glMaterial_Ambient;
  GLint glMaterial_Diffuse;
  GLint glMaterial_Specular;
  GLint glMaterial_SpecularExp;

  GLint glMaterial_Emission;
  GLint glMaterial_Refraction;
  GLint glMaterial_IlluminationModel;
  GLint glMaterial_HeightScale;

  GLint glMaterial_SamplerAmbient;
  GLint glMaterial_SamplerDiffuse;
  GLint glMaterial_SamplerSpecular;

  GLint glMaterial_SamplerSpecularExp;
  GLint glMaterial_SamplerDissolve;
  GLint glMaterial_SamplerBump;
  GLint glMaterial_SamplerDisplacement;

  GLint glMaterial_HasTextureAmbient;
  GLint glMaterial_HasTextureDiffuse;
  GLint glMaterial_HasTextureSpecular;

  GLint glMaterial_HasTextureSpecularExp;
  GLint glMaterial_HasTextureDissolve;
  GLint glMaterial_HasTextureBump;
  GLint glMaterial_HasTextureDisplacement;

  GLint glMaterial_ParallaxMapping;

  // effects - gaussian blur
  GLint glEffect_GB_W;
  GLint glEffect_GB_Radius;
  GLint glEffect_GB_Mode;

  // effects - bloom
  GLint glEffect_Bloom_doBloom;
  GLint glEffect_Bloom_WeightA;
  GLint glEffect_Bloom_WeightB;
  GLint glEffect_Bloom_WeightC;
  GLint glEffect_Bloom_WeightD;
  GLint glEffect_Bloom_Vignette;
  GLint glEffect_Bloom_VignetteAtt;

  // effects - tone mapping
  GLint glEffect_ToneMapping_ACESFilmRec2020;

  // view skin
  std::unique_ptr<ModelFace_LightSource_Directional> solidLight;
  GLint glFS_solidSkin_materialColor;
};

#endif /* RenderingForwardShadowMapping_hpp */
