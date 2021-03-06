//
//  ModelFaceBase.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceBase_hpp
#define ModelFaceBase_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/objects/Objects.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/helpers/Light.hpp"
#include "kuplung/meshes/helpers/WorldGrid.hpp"
#include "kuplung/utilities/maths/Maths.hpp"
#include "kuplung/meshes/helpers/BoundingBox.hpp"
#include "kuplung/meshes/helpers/VertexSphere.hpp"

class ModelFaceBase {
public:
  virtual ~ModelFaceBase();
  ModelFaceBase* clone(const int modelID);
  virtual void init(MeshModel const& model, std::string const& assetsFolder);
  void initModelProperties();
  void initBoundingBox();
  void initVertexSphere();
  virtual void initBuffers();
  void loadTexture(std::string const& assetsFolder, MeshMaterialTextureImage const& materialImage, const objMaterialImageType& type, GLuint* vboObject);

  virtual void render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const glm::mat4& matrixModel, const glm::vec3& vecCameraPosition, WorldGrid *grid, const glm::vec3& uiAmbientLight);

  // general options
  void setOptionsFOV(float fov);

  // outlining
  void setOptionsSelected(bool selectedYn);
  void setOptionsOutlineColor(const glm::vec4& outlineColor);
  void setOptionsOutlineThickness(float thickness);
  const bool getOptionsSelected() const;
  const glm::vec4 getOptionsOutlineColor() const;
  const float getOptionsOutlineThickness() const;

  std::unique_ptr<BoundingBox> boundingBox;
  std::unique_ptr<VertexSphere> vertexSphere;
  bool initBuffersAgain;
  MeshModel meshModel;
  int ModelID;
  glm::mat4 matrixProjection, matrixCamera, matrixModel;
  glm::vec3 vecCameraPosition;
  std::vector<Light*> lightSources;
  std::vector<GLfloat> dataVertices;
  std::vector<GLfloat> dataTexCoords;
  std::vector<GLfloat> dataNormals;
  std::vector<GLuint> dataIndices;
  std::string assetsFolder;

  bool Settings_DeferredRender;
  bool Setting_CelShading, Setting_Wireframe, Setting_UseTessellation, Setting_UseCullFace;
  float Setting_Alpha;
  int Setting_TessellationSubdivision;
  std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
  bool scale0;
  std::unique_ptr<ObjectCoordinate> scaleX, scaleY, scaleZ;
  std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
  std::unique_ptr<ObjectCoordinate> displaceX, displaceY, displaceZ;
  std::unique_ptr<ObjectCoordinate> Setting_MaterialRefraction;
  std::unique_ptr<ObjectCoordinate> Setting_MaterialSpecularExp;

  // Gizmo controls
  bool Setting_Gizmo_Translate, Setting_Gizmo_Rotate, Setting_Gizmo_Scale;
  bool Setting_EditMode;

  // view skin
  ViewModelSkin Setting_ModelViewSkin;
  glm::vec3 solidLightSkin_MaterialColor, solidLightSkin_Ambient, solidLightSkin_Diffuse, solidLightSkin_Specular;
  float solidLightSkin_Ambient_Strength, solidLightSkin_Diffuse_Strength, solidLightSkin_Specular_Strength;

  // light
  glm::vec3 Setting_LightPosition, Setting_LightDirection;
  glm::vec3 Setting_LightAmbient, Setting_LightDiffuse, Setting_LightSpecular;
  float Setting_LightStrengthAmbient, Setting_LightStrengthDiffuse, Setting_LightStrengthSpecular;

  // material
  unsigned int materialIlluminationModel;
  std::unique_ptr<ObjectCoordinate> displacementHeightScale;
  bool showMaterialEditor;
  std::unique_ptr<MaterialColor> materialAmbient, materialDiffuse, materialSpecular, materialEmission;

  // mapping
  bool Setting_ParallaxMapping;

  // effects - gaussian blur
  int Effect_GBlur_Mode;
  std::unique_ptr<ObjectCoordinate> Effect_GBlur_Radius, Effect_GBlur_Width;

  // effects - bloom
  bool Effect_Bloom_doBloom;
  float Effect_Bloom_WeightA, Effect_Bloom_WeightB, Effect_Bloom_WeightC, Effect_Bloom_WeightD, Effect_Bloom_Vignette, Effect_Bloom_VignetteAtt;

  // effects - tone mapping
  bool Effect_ToneMapping_ACESFilmRec2020, Effect_HDR_Tonemapping;

  // rendering
  int Setting_LightingPass_DrawMode;

  // shadows
  bool Setting_ShowShadows;

  // PBR
  bool Setting_Rendering_PBR;
  float Setting_Rendering_PBR_Metallic;
  float Setting_Rendering_PBR_Roughness;
  float Setting_Rendering_PBR_AO;

protected:
  float so_fov;
  float so_outlineThickness;
  glm::vec4 so_outlineColor;
  bool so_selectedYn;
  glm::vec3 uiAmbientLight;
  WorldGrid *grid;

    // TODO: get this out of here!
  std::unique_ptr<KuplungApp::Utilities::Math::Maths> mathHelper;
};

#endif /* ModelFaceBase_hpp */
