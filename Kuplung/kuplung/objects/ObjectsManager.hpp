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
#include "kuplung/meshes/helpers/MiniAxis.hpp"
#include "kuplung/meshes/helpers/Skybox.hpp"
#include "kuplung/meshes/helpers/WorldGrid.hpp"
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
    ObjectsManager();

    void destroy();
    void init(std::function<void(float)> doProgress,
              std::function<void()> addTerrain,
              std::function<void()> addSpaceship);
    void loadSystemModels(std::unique_ptr<FileModelManager> &fileParser);
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
    void initAxisSystem();
    void initAxisHelpers();
    void initSkybox();
    void initTerrain();
    void initSpaceship();

    void addLight(LightSourceType type, std::string title = "", std::string description = "");
    std::vector<Light*> lightSources;

    Camera *camera;
    CameraModel *cameraModel;
    WorldGrid *grid;
    MiniAxis *axisSystem;
    Skybox *skybox;
    AxisHelpers *axisHelpers_xMinus, *axisHelpers_xPlus, *axisHelpers_yMinus, *axisHelpers_yPlus, *axisHelpers_zMinus, *axisHelpers_zPlus;
    Terrain *terrain;
    Spaceship *spaceship;

    glm::mat4 matrixProjection;
    float Setting_FOV = 45.0;
    float Setting_OutlineThickness = 1.01f;
    float Setting_RatioWidth = 4.0f, Setting_RatioHeight = 3.0f;
    float Setting_PlaneClose = 0.1f, Setting_PlaneFar = 100.0f;
    int Setting_GridSize = 30, Setting_Skybox = 0;
    glm::vec4 Setting_OutlineColor;
    glm::vec3 Setting_UIAmbientLight;
    bool Setting_FixedGridWorld = true, Setting_OutlineColorPickerOpen = false, Setting_ShowAxisHelpers = true;
    bool Settings_ShowZAxis = true;
    bool Setting_DeferredTestMode, Setting_DeferredTestLights;
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

    bool Setting_ShowSpaceship, Setting_GenerateSpaceship;

    bool Setting_VertexSphere_Visible, Setting_VertexSphere_ColorPickerOpen;
    bool Setting_VertexSphere_IsSphere, Setting_VertexSphere_ShowWireframes;
    float Setting_VertexSphere_Radius;
    int Setting_VertexSphere_Segments;
    glm::vec4 Setting_VertexSphere_Color;
    int VertexEditorModeID;
    glm::vec3 VertexEditorMode;
    GeometryEditMode Setting_GeometryEditMode;

    std::map<std::string, MeshModel> systemModels;

private:
    std::function<void(float)> funcProgress;
    std::function<void()> funcAddTerrain;
    std::function<void()> funcAddSpaceship;
};

#endif /* ObjectsManager_hpp */
