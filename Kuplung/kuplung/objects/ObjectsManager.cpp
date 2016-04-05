//
//  ObjectsManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ObjectsManager.hpp"

/*
 *
 * Constructor & Destructor
 *
 */
ObjectsManager::ObjectsManager(FileModelManager *parser) {
    this->fileParser = parser;
}

ObjectsManager::~ObjectsManager() {
    this->destroy();
}

/*
 *
 * General
 *
 */

void ObjectsManager::destroy() {
    this->fileParser->destroy();
    this->camera->destroy();
    this->grid->destroy();
    this->axisSystem->destroy();
    this->skybox->destroy();
    for (size_t i=0; i<this->lightSources.size(); i++) {
        this->lightSources[i]->destroy();
    }
}

void ObjectsManager::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;

    this->lightSources.clear();
    this->systemModels.clear();

    this->resetSettings();
}

void ObjectsManager::render() {
    this->matrixProjection = glm::perspective(glm::radians(this->Setting_FOV), this->Setting_RatioWidth / this->Setting_RatioHeight, this->Setting_PlaneClose, this->Setting_PlaneFar);

    this->camera->render();

    if (this->Setting_GridSize != this->grid->gridSize) {
        this->grid->gridSize = this->Setting_GridSize;
        this->grid->initBuffers(this->Setting_GridSize, 1);
        this->skybox->init(this->Setting_GridSize);
    }

    this->grid->render(this->matrixProjection, this->camera->matrixCamera);
    this->axisSystem->render(this->matrixProjection, this->camera->matrixCamera);

    for (size_t i=0; i<this->lightSources.size(); i++) {
        this->lightSources[i]->render(this->matrixProjection, this->camera->matrixCamera, this->grid->matrixModel, this->Setting_FixedGridWorld);
    }

    this->skybox->render(this->camera->matrixCamera, this->Setting_PlaneClose, this->Setting_PlaneFar, this->Setting_FOV);
}

void ObjectsManager::resetSettings() {
    this->Setting_FOV = 45.0;
    this->Setting_Alpha = 1;
    this->Setting_OutlineThickness = 1.01;
    this->Setting_RatioWidth = 4.0f;
    this->Setting_RatioHeight = 3.0f;
    this->Setting_PlaneClose = 0.1f;
    this->Setting_PlaneFar = 100.0f;
    this->Setting_GridSize = 10;
    this->Setting_OutlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->Setting_FixedGridWorld = true;
    this->Setting_OutlineColorPickerOpen = false;
}

void ObjectsManager::resetPropertiesSystem() {
    this->resetSettings();

    if (this->camera)
        this->camera->initProperties();
    if (this->grid)
        this->grid->initProperties(this->Setting_GridSize);
    if (this->axisSystem)
        this->axisSystem->initProperties();
    for (size_t i=0; i<this->lightSources.size(); i++) {
        this->lightSources[i]->initProperties();
    }
}

/*
 *
 * Camera
 *
 */
void ObjectsManager::initCamera() {
    this->camera = new Camera();
    this->camera->initProperties();
}

/*
 *
 * Grid
 *
 */
void ObjectsManager::initGrid() {
    this->grid = new WorldGrid();
    this->grid->init();
    this->grid->initShaderProgram();
    this->grid->initBuffers(10, 1);

}

/*
 *
 * Axis window
 *
 */
void ObjectsManager::initAxisSystem() {
    this->axisSystem = new CoordinateSystem();
    this->axisSystem->init();
    this->axisSystem->initShaderProgram();
    this->axisSystem->initBuffers();
}

/*
 *
 * Skybox
 *
 */
void ObjectsManager::initSkybox() {
    this->skybox = new Skybox();
    if (!this->skybox->init(this->Setting_GridSize))
        Settings::Instance()->funcDoLog("Skybox cannot be initialized!");
}

/*
 *
 * Lights
 *
 */
void ObjectsManager::addLight(LightSourceType type, std::string title, std::string description) {
    Light *lightObject = new Light();
    lightObject->init(type);
    lightObject->type = type;
    switch (type) {
        case LightSourceType_Directional:
            lightObject->title = ((title == "") ? "Directional " + std::to_string((int)this->lightSources.size() + 1) : title);
            lightObject->description = ((description == "") ? "Directional area light source" : description);
            lightObject->setModel(this->systemModels["light_directional"].models[0].faces[0]);
            break;
        case LightSourceType_Point:
            lightObject->title = ((title == "") ? "Point " + std::to_string((int)this->lightSources.size() + 1) : title);
            lightObject->description = ((description == "") ? "Omnidirectional point light source" : description);
            lightObject->setModel(this->systemModels["light_point"].models[0].faces[0]);
            break;
        case LightSourceType_Spot:
            lightObject->title = ((title == "") ? "Spot " + std::to_string((int)this->lightSources.size() + 1) : title);
            lightObject->description = ((description == "") ? "Directional cone light source" : description);
            lightObject->setModel(this->systemModels["light_spot"].models[0].faces[0]);
            break;
        default:
            lightObject->setModel(this->systemModels["lamp"].models[0].faces[0]);
            break;
    }
    lightObject->initShaderProgram();
    lightObject->initBuffers(std::string(Settings::Instance()->appFolder()));

    this->lightSources.push_back(lightObject);
}

/*
 *
 * Utilities
 *
 */
void ObjectsManager::loadSystemModels() {
    // lamp
    FBEntity file;
    file.isFile = true;
    file.extension = ".obj";
    file.title = "light";
    file.path = Settings::Instance()->appFolder() + "/gui/light.obj";
    this->systemModels["lamp"] = this->fileParser->parse(file);

    file.title = "light_directional";
    file.path = Settings::Instance()->appFolder() + "/gui/light_directional.obj";
    this->systemModels["light_directional"] = this->fileParser->parse(file);

    file.title = "light_point";
    file.path = Settings::Instance()->appFolder() + "/gui/light_point.obj";
    this->systemModels["light_point"] = this->fileParser->parse(file);

    file.title = "light_spot";
    file.path = Settings::Instance()->appFolder() + "/gui/light_spot.obj";
    this->systemModels["light_spot"] = this->fileParser->parse(file);
}
