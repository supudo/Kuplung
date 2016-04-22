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
    this->cameraModel->destroy();
    this->grid->destroy();
    this->axisHelpers_yMinus->destroy();
    this->axisHelpers_yPlus->destroy();
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
        this->axisHelpers_xMinus->initBuffers();
        this->axisHelpers_xPlus->initBuffers();
        this->axisHelpers_yMinus->initBuffers();
        this->axisHelpers_yPlus->initBuffers();
        this->axisHelpers_zMinus->initBuffers();
        this->axisHelpers_zPlus->initBuffers();
    }

    this->grid->render(this->matrixProjection, this->camera->matrixCamera);
    if (this->Setting_ShowAxisHelpers) {
        int gridSize = this->Setting_GridSize;
        float x = this->camera->positionX->point;
        float y = this->camera->positionY->point;
        float z = this->camera->positionZ->point;
        this->axisHelpers_xMinus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(- gridSize / 2 - 1, 0, 0), x, y, z);
        this->axisHelpers_xPlus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(gridSize / 2, 0, 0), x, y, z);
        this->axisHelpers_yMinus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, - gridSize / 2 - 1, 0), x, y, z);
        this->axisHelpers_yPlus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, gridSize / 2, 0), x, y, z);
        this->axisHelpers_zMinus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, 0, - gridSize / 2 - 1), x, y, z);
        this->axisHelpers_zPlus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, 0, gridSize / 2), x, y, z);
    }
    this->axisSystem->render(this->matrixProjection, this->camera->matrixCamera);

    this->cameraModel->render(this->matrixProjection, this->camera->matrixCamera, this->grid->matrixModel, this->Setting_FixedGridWorld);

    for (size_t i=0; i<this->lightSources.size(); i++) {
        this->lightSources[i]->render(this->matrixProjection, this->camera->matrixCamera, this->grid->matrixModel, this->Setting_FixedGridWorld);
    }

    if (this->Setting_Skybox != this->skybox->Setting_Skybox_Item) {
        this->skybox->initBuffers();
        this->Setting_Skybox = this->skybox->Setting_Skybox_Item;
    }
    this->skybox->render(this->camera->matrixCamera, this->Setting_PlaneClose, this->Setting_PlaneFar, this->Setting_FOV);
}

void ObjectsManager::resetSettings() {
    this->Setting_FOV = 45.0;
    this->Setting_Alpha = 1;
    this->Setting_OutlineThickness = 0.0f;
    this->Setting_RatioWidth = 4.0f;
    this->Setting_RatioHeight = 3.0f;
    this->Setting_PlaneClose = 0.1f;
    this->Setting_PlaneFar = 100.0f;
    this->Setting_GridSize = 10;
    this->Setting_OutlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->Setting_UIAmbientLight = glm::vec3(1.0f);
    this->Setting_FixedGridWorld = true;
    this->Setting_OutlineColorPickerOpen = false;
    this->Setting_ShowAxisHelpers = true;
}

void ObjectsManager::resetPropertiesSystem() {
    this->resetSettings();

    if (this->camera)
        this->camera->initProperties();
    if (this->cameraModel)
        this->cameraModel->initProperties();
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
 * Camera Model
 *
 */
void ObjectsManager::initCameraModel() {
    this->cameraModel = new CameraModel();
    this->cameraModel->initProperties();
    this->cameraModel->initShaderProgram();
    this->cameraModel->setModel(this->systemModels["camera"].models[0].faces[0]);
    this->cameraModel->initBuffers();
}

/*
 *
 * Axis window
 *
 */
void ObjectsManager::initAxisSystem() {
    this->axisSystem = new MiniAxis();
    this->axisSystem->init();
    this->axisSystem->initShaderProgram();
    this->axisSystem->initBuffers();
}

/*
 *
 * Axis Helpers
 *
 */
void ObjectsManager::initAxisHelpers() {
    this->axisHelpers_xMinus = new AxisHelpers();
    this->axisHelpers_xMinus->init();
    this->axisHelpers_xMinus->setModel(this->systemModels["axis_x_minus"].models[0].faces[0]);
    this->axisHelpers_xMinus->initShaderProgram();
    this->axisHelpers_xMinus->initBuffers();

    this->axisHelpers_xPlus = new AxisHelpers();
    this->axisHelpers_xPlus->init();
    this->axisHelpers_xPlus->setModel(this->systemModels["axis_x_plus"].models[0].faces[0]);
    this->axisHelpers_xPlus->initShaderProgram();
    this->axisHelpers_xPlus->initBuffers();

    this->axisHelpers_yMinus = new AxisHelpers();
    this->axisHelpers_yMinus->init();
    this->axisHelpers_yMinus->setModel(this->systemModels["axis_y_minus"].models[0].faces[0]);
    this->axisHelpers_yMinus->initShaderProgram();
    this->axisHelpers_yMinus->initBuffers();

    this->axisHelpers_yPlus = new AxisHelpers();
    this->axisHelpers_yPlus->init();
    this->axisHelpers_yPlus->setModel(this->systemModels["axis_y_plus"].models[0].faces[0]);
    this->axisHelpers_yPlus->initShaderProgram();
    this->axisHelpers_yPlus->initBuffers();

    this->axisHelpers_zMinus = new AxisHelpers();
    this->axisHelpers_zMinus->init();
    this->axisHelpers_zMinus->setModel(this->systemModels["axis_z_minus"].models[0].faces[0]);
    this->axisHelpers_zMinus->initShaderProgram();
    this->axisHelpers_zMinus->initBuffers();

    this->axisHelpers_zPlus = new AxisHelpers();
    this->axisHelpers_zPlus->init();
    this->axisHelpers_zPlus->setModel(this->systemModels["axis_z_plus"].models[0].faces[0]);
    this->axisHelpers_zPlus->initShaderProgram();
    this->axisHelpers_zPlus->initBuffers();
}

/*
 *
 * Skybox
 *
 */
void ObjectsManager::initSkybox() {
    this->skybox = new Skybox();
    this->skybox->init(this->Setting_GridSize);
    if (!this->skybox->initBuffers())
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
    FBEntity file;
    file.isFile = true;
    file.extension = ".obj";

    file.title = "light";
    file.path = Settings::Instance()->appFolder() + "/gui/light.obj";
    this->systemModels["lamp"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "light_directional";
    file.path = Settings::Instance()->appFolder() + "/gui/light_directional.obj";
    this->systemModels["light_directional"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "light_point";
    file.path = Settings::Instance()->appFolder() + "/gui/light_point.obj";
    this->systemModels["light_point"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "light_spot";
    file.path = Settings::Instance()->appFolder() + "/gui/light_spot.obj";
    this->systemModels["light_spot"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "camera";
    file.path = Settings::Instance()->appFolder() + "/gui/camera.obj";
    this->systemModels["camera"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "x_plus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/x_plus.obj";
    this->systemModels["axis_x_plus"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "x_minus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/x_minus.obj";
    this->systemModels["axis_x_minus"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "y_plus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/y_plus.obj";
    this->systemModels["axis_y_plus"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "y_minus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/y_minus.obj";
    this->systemModels["axis_y_minus"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "z_plus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/z_plus.obj";
    this->systemModels["axis_z_plus"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);

    file.title = "z_minus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/z_minus.obj";
    this->systemModels["axis_z_minus"] = this->fileParser->parse(file, FileBrowser_ParserType_Assimp);
}
