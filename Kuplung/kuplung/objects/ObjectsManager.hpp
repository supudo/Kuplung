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
#include "kuplung/meshes/Camera.hpp"
#include "kuplung/meshes/CameraModel.hpp"
#include "kuplung/meshes/Light.hpp"
#include "kuplung/meshes/AxisHelpers.hpp"
#include "kuplung/meshes/MiniAxis.hpp"
#include "kuplung/meshes/Skybox.hpp"
#include "kuplung/meshes/WorldGrid.hpp"
#include "kuplung/utilities/parsers/FileModelManager.hpp"

class ObjectsManager {
public:
    ~ObjectsManager();
    ObjectsManager(FileModelManager *parser);

    void destroy();
    void init(std::function<void(float)> doProgress);
    void loadSystemModels();
    void render();
    void resetPropertiesSystem();
    void resetSettings();

    void initCamera();
    void initCameraModel();
    void initGrid();
    void initAxisSystem();
    void initAxisHelpers();
    void initSkybox();

    void addLight(LightSourceType type, std::string title = "", std::string description = "");
    std::vector<Light*> lightSources;

    FileModelManager *fileParser;
    Camera* camera;
    CameraModel* cameraModel;
    WorldGrid* grid;
    MiniAxis* axisSystem;
    Skybox* skybox;
    AxisHelpers *axisHelpers_xMinus, *axisHelpers_xPlus, *axisHelpers_yMinus, *axisHelpers_yPlus, *axisHelpers_zMinus, *axisHelpers_zPlus;

    glm::mat4 matrixProjection;
    float Setting_FOV = 45.0;
    float Setting_Alpha = 1;
    float Setting_OutlineThickness = 1.01;
    float Setting_RatioWidth = 4.0f, Setting_RatioHeight = 3.0f;
    float Setting_PlaneClose = 0.1f, Setting_PlaneFar = 100.0f;
    int Setting_GridSize = 10, Setting_Skybox = 0;
    glm::vec4 Setting_OutlineColor;
    glm::vec3 Setting_UIAmbientLight;
    bool Setting_FixedGridWorld = true, Setting_OutlineColorPickerOpen = false, Setting_ShowAxisHelpers = true;

private:
    std::function<void(float)> funcProgress;

    std::map<std::string, objScene> systemModels;
};

#endif /* ObjectsManager_hpp */
