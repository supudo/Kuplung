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
#include "kuplung/meshes/Light.hpp"
#include "kuplung/meshes/CoordinateSystem.hpp"
#include "kuplung/meshes/WorldGrid.hpp"
#include "kuplung/utilities/parsers/FileModelManager.hpp"

class ObjectsManager {
public:
    ~ObjectsManager();
    ObjectsManager(FileModelManager *parser);

    void destroy();
    void init(std::function<void(std::string)> doLog, std::function<void(float)> doProgress);
    void loadSystemModels();
    void render();
    void resetPropertiesSystem();
    void resetPropertiesModels();
    void resetSettings();

    void initCamera();
    void initGrid();
    void initAxisSystem();

    void addLight(LightSourceType type, std::string title = "", std::string description = "");
    std::vector<Light*> lightSources;

    FileModelManager *fileParser;
    Camera* camera;
    WorldGrid* grid;
    CoordinateSystem* axisSystem;

    glm::mat4 matrixProjection;
    float Setting_FOV = 45.0;
    float Setting_Alpha = 1;
    float Setting_OutlineThickness = 1.01;
    float Setting_RatioWidth = 4.0f, Setting_RatioHeight = 3.0f;
    float Setting_PlaneClose = 0.1f, Setting_PlaneFar = 100.0f;
    int Setting_GridSize = 10;
    glm::vec4 Setting_OutlineColor;
    bool Setting_FixedGridWorld = true, Setting_OutlineColorPickerOpen = false;

private:
    std::function<void(std::string)> funcLog;
    std::function<void(float)> funcProgress;

    void logMessage(std::string message);
    std::map<std::string, objScene> systemModels;
};

#endif /* ObjectsManager_hpp */
