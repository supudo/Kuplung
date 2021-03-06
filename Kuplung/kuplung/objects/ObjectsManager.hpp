//
//  ObjectsManager.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ObjectsManager_hpp
#define ObjectsManager_hpp

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/helpers/Camera.hpp"
#include "kuplung/meshes/helpers/CameraModel.hpp"
#include "kuplung/meshes/helpers/Light.hpp"
#include "kuplung/meshes/helpers/AxisHelpers.hpp"
#include "kuplung/meshes/helpers/AxisLabels.hpp"
#include "kuplung/meshes/helpers/MiniAxis.hpp"
#include "kuplung/meshes/helpers/Skybox.hpp"
#include "kuplung/meshes/helpers/WorldGrid.hpp"
#include "kuplung/meshes/helpers/Grid2D.hpp"
#include "kuplung/meshes/artefacts/Terrain.hpp"
#include "kuplung/meshes/artefacts/Spaceship.hpp"
#include "kuplung/utilities/parsers/FileModelManager.hpp"

typedef enum GeometryEditMode {
  GeometryEditMode_Vertex,
  GeometryEditMode_Line,
  GeometryEditMode_Face
} GeometryEditMode;

class ObjectsManager {
public:
  ~ObjectsManager();
  void init(const std::function<void(float)>& doProgress,
            const std::function<void()>& addTerrain,
            const std::function<void()>& addSpaceship);
  void loadSystemModels(const std::unique_ptr<FileModelManager> &fileParser);
  void render();
  void renderSkybox();
  void resetPropertiesSystem();
  void resetSettings();
  void generateTerrain();
  void generateSpaceship();
  void clearAllLights();

  void initCamera();
  void initCameraModel();
  void initGrid();
  void initGrid2D();
  void initAxisSystem();
  void initAxisHelpers();
  void initAxisLabels();
  void initSkybox();
  void initTerrain();
  void initSpaceship();

  void addLight(const LightSourceType type, std::string const& title = "", std::string const& description = "");
  std::vector<Light*> lightSources;

  std::unique_ptr<Camera> camera;
	std::unique_ptr<CameraModel> cameraModel;
	std::unique_ptr<WorldGrid> grid;
	std::unique_ptr<Grid2D> grid2d;
	std::unique_ptr<MiniAxis> axisSystem;
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<AxisHelpers> axisHelpers_xMinus, axisHelpers_xPlus, axisHelpers_yMinus, axisHelpers_yPlus, axisHelpers_zMinus, axisHelpers_zPlus;
  std::unique_ptr<AxisLabels> axisLabels;
	std::unique_ptr<Terrain> terrain;
	std::unique_ptr<Spaceship> spaceship;

  glm::mat4 matrixProjection;
  float Setting_FOV = 45.0;
  float Setting_OutlineThickness = 1.01f;
  float Setting_RatioWidth = 4.0f, Setting_RatioHeight = 3.0f;
  float Setting_PlaneClose = 0.1f, Setting_PlaneFar = 100.0f;
  int Setting_GridSize = 30, Setting_Skybox = 0;
  glm::vec4 Setting_OutlineColor;
  glm::vec3 Setting_UIAmbientLight;
  bool Setting_FixedGridWorld = true, Setting_OutlineColorPickerOpen = false, Setting_ShowAxisHelpers = true;
  bool Setting_UseWorldGrid = false;
  bool Settings_ShowZAxis = true;
  bool Setting_DeferredTestMode, Setting_DeferredTestLights, Setting_DeferredRandomizeLightPositions;
  int Setting_LightingPass_DrawMode, Setting_DeferredTestLightsNumber;
  float Setting_DeferredAmbientStrength;
  float Setting_GammaCoeficient;

  ViewModelSkin viewModelSkin;
  glm::vec3 SolidLight_Direction;
  glm::vec3 SolidLight_MaterialColor, SolidLight_Ambient, SolidLight_Diffuse, SolidLight_Specular;
  float SolidLight_Ambient_Strength, SolidLight_Diffuse_Strength, SolidLight_Specular_Strength;
  bool SolidLight_MaterialColor_ColorPicker, SolidLight_Ambient_ColorPicker, SolidLight_Diffuse_ColorPicker, SolidLight_Specular_ColorPicker;

  bool Setting_ShowTerrain, Setting_TerrainModel, Setting_TerrainAnimateX, Setting_TerrainAnimateY;
  std::string heightmapImage;
  int Setting_TerrainWidth = 100, Setting_TerrainHeight = 100;

  bool Setting_ShowSpaceship, Setting_GenerateSpaceship, Setting_Cuda_ShowOceanFFT;

  bool Setting_VertexSphere_Visible, Setting_VertexSphere_ColorPickerOpen;
  bool Setting_VertexSphere_IsSphere, Setting_VertexSphere_ShowWireframes;
  float Setting_VertexSphere_Radius;
  int Setting_VertexSphere_Segments;
  glm::vec4 Setting_VertexSphere_Color;
  int VertexEditorModeID;
  glm::vec3 VertexEditorMode;
  GeometryEditMode Setting_GeometryEditMode;

  std::map<std::string, MeshModel> systemModels;

  bool Setting_Rendering_Depth, Setting_DebugShadowTexture;

	std::string shaderSourceVertex, shaderSourceTCS, shaderSourceTES, shaderSourceGeometry, shaderSourceFragment;

private:
  std::function<void(float)> funcProgress;
  std::function<void()> funcAddTerrain;
  std::function<void()> funcAddSpaceship;
};

#endif /* ObjectsManager_hpp */
