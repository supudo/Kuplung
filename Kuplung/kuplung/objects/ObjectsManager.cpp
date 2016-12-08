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

ObjectsManager::~ObjectsManager() {
    delete this->camera;
    delete this->cameraModel;
    delete this->grid;
    delete this->axisHelpers_xMinus;
    delete this->axisHelpers_xPlus;
    delete this->axisHelpers_yMinus;
    delete this->axisHelpers_yPlus;
    delete this->axisHelpers_zMinus;
    delete this->axisHelpers_zPlus;
    delete this->axisSystem;
    delete this->skybox;
    for (size_t i=0; i<this->lightSources.size(); i++) {
        delete this->lightSources[i];
    }
}

void ObjectsManager::init(std::function<void(float)> doProgress, std::function<void()> addTerrain, std::function<void()> addSpaceship) {
    this->funcProgress = doProgress;
    this->funcAddTerrain = addTerrain;
    this->funcAddSpaceship = addSpaceship;

    this->lightSources.clear();
    this->systemModels.clear();

    this->viewModelSkin = ViewModelSkin_Rendered;
    //this->viewModelSkin = ViewModelSkin_Texture;

    this->resetSettings();
}

void ObjectsManager::render() {
    this->matrixProjection = glm::perspective(glm::radians(this->Setting_FOV), this->Setting_RatioWidth / this->Setting_RatioHeight, this->Setting_PlaneClose, this->Setting_PlaneFar);

    this->camera->render();

    if (Settings::Instance()->showAllVisualArtefacts) {
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

        this->grid->render(this->matrixProjection, this->camera->matrixCamera, this->Settings_ShowZAxis);

        if (this->Setting_ShowAxisHelpers) {
            float ahPosition = 0;
            ahPosition = this->Setting_GridSize;
            ahPosition /= 2;
            //ahPosition += 1;

            this->axisHelpers_xMinus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(- ahPosition, 0, 0));
            this->axisHelpers_xPlus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(ahPosition, 0, 0));

            this->axisHelpers_yMinus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, - ahPosition, 0));
            this->axisHelpers_yPlus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, ahPosition, 0));

            this->axisHelpers_zMinus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, 0, - ahPosition));
            this->axisHelpers_zPlus->render(this->matrixProjection, this->camera->matrixCamera, glm::vec3(0, 0, ahPosition));
        }
        this->axisSystem->render(this->matrixProjection, this->camera->matrixCamera);

        this->cameraModel->render(this->matrixProjection, this->camera->matrixCamera, this->grid->matrixModel, this->Setting_FixedGridWorld);

        for (size_t i=0; i<this->lightSources.size(); i++) {
            this->lightSources[i]->render(this->matrixProjection, this->camera->matrixCamera);
        }

        if (this->Setting_Skybox != this->skybox->Setting_Skybox_Item) {
            this->skybox->initBuffers();
            this->Setting_Skybox = this->skybox->Setting_Skybox_Item;
        }
        this->renderSkybox();

        if (this->Setting_TerrainAnimateX)
            this->terrain->terrainGenerator->Setting_OffsetHorizontal += 0.0001f;
        if (this->Setting_TerrainAnimateY)
            this->terrain->terrainGenerator->Setting_OffsetVertical += 0.0001f;
        if (this->Setting_TerrainAnimateX || this->Setting_TerrainAnimateY)
            this->generateTerrain();

        if (this->Setting_ShowTerrain) {
            if (this->heightmapImage == "")
                this->generateTerrain();
            this->terrain->render(this->matrixProjection, this->camera->matrixCamera, this->grid->matrixModel);
            this->heightmapImage = this->terrain->heightmapImage;
        }

        if (this->Setting_TerrainModel) {
            this->funcAddTerrain();
            this->Setting_TerrainModel = false;
        }

        if (this->Setting_ShowSpaceship) {
            if (this->Setting_GenerateSpaceship) {
                this->generateSpaceship();
                this->funcAddSpaceship();
                this->Setting_GenerateSpaceship = false;
            }

//            this->spaceship->solidLightSkin_MaterialColor = this->SolidLight_MaterialColor;
//            bool doSolidLight = true;
//            for (size_t i=0; i<this->lightSources.size(); i++) {
//                if (this->lightSources[i]->type == LightSourceType_Directional) {
//                    this->spaceship->lightDirection = glm::vec3(this->lightSources[i]->positionX->point, this->lightSources[i]->positionY->point, this->lightSources[i]->positionZ->point);
//                    this->spaceship->solidLightSkin_Ambient = this->lightSources[i]->ambient->color;
//                    this->spaceship->solidLightSkin_Diffuse = this->lightSources[i]->diffuse->color;
//                    this->spaceship->solidLightSkin_Specular = this->lightSources[i]->specular->color;
//                    this->spaceship->solidLightSkin_Ambient_Strength = this->lightSources[i]->ambient->strength;
//                    this->spaceship->solidLightSkin_Diffuse_Strength = this->lightSources[i]->diffuse->strength;
//                    this->spaceship->solidLightSkin_Specular_Strength = this->lightSources[i]->specular->strength;
//                    doSolidLight = false;
//                    break;
//                }
//            }
//            if (doSolidLight) {
//                this->spaceship->lightDirection = this->camera->cameraPosition;
//                this->spaceship->solidLightSkin_Ambient = this->SolidLight_Ambient;
//                this->spaceship->solidLightSkin_Diffuse = this->SolidLight_Diffuse;
//                this->spaceship->solidLightSkin_Specular = this->SolidLight_Specular;
//                this->spaceship->solidLightSkin_Ambient_Strength = this->SolidLight_Ambient_Strength;
//                this->spaceship->solidLightSkin_Diffuse_Strength = this->SolidLight_Diffuse_Strength;
//                this->spaceship->solidLightSkin_Specular_Strength = this->SolidLight_Specular_Strength;
//            }
//            this->spaceship->render(this->matrixProjection, this->camera->matrixCamera, this->grid->matrixModel, this->camera->cameraPosition);
        }
    }
}

void ObjectsManager::renderSkybox() {
    this->skybox->render(this->camera->matrixCamera, this->Setting_PlaneClose, this->Setting_PlaneFar, this->Setting_FOV);
}

void ObjectsManager::resetSettings() {
    this->Setting_FOV = 45.0;
    this->Setting_OutlineThickness = 0.0f;
    this->Setting_RatioWidth = 4.0f;
    this->Setting_RatioHeight = 3.0f;
    this->Setting_PlaneClose = 1.0f;
    this->Setting_PlaneFar = 1000.0f;
    this->Setting_GridSize = 30;
    this->Setting_OutlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->Setting_UIAmbientLight = glm::vec3(0.2f);
    this->Setting_FixedGridWorld = true;
    this->Setting_OutlineColorPickerOpen = false;
    this->Setting_ShowAxisHelpers = true;
    this->Settings_ShowZAxis = true;
    this->SolidLight_Direction = glm::vec3(0.0f, 1.0f, 0.0f);
    this->SolidLight_MaterialColor = glm::vec3(0.7f);
    this->SolidLight_Ambient = glm::vec3(1.0);
    this->SolidLight_Diffuse = glm::vec3(1.0);
    this->SolidLight_Specular = glm::vec3(1.0);
    this->SolidLight_Ambient_Strength = 0.3f;
    this->SolidLight_Diffuse_Strength = 1.0;
    this->SolidLight_Specular_Strength = 0.0;
    this->SolidLight_Ambient_ColorPicker = false;
    this->SolidLight_Diffuse_ColorPicker = false;
    this->SolidLight_Specular_ColorPicker = false;
    this->Setting_ShowTerrain = false;
    this->Setting_TerrainModel = false;
    this->Setting_TerrainAnimateX = false;
    this->Setting_TerrainAnimateY = false;
    this->Setting_TerrainWidth = 100;
    this->Setting_TerrainHeight = 100;
    this->Setting_DeferredTestMode = false;
    this->Setting_DeferredTestLights = false;
    this->Setting_LightingPass_DrawMode = 0;
    this->Setting_DeferredAmbientStrength = 0.1f;
    this->Setting_DeferredTestLightsNumber = 32;
    this->Setting_ShowSpaceship = false;
    this->Setting_GenerateSpaceship = false;
    this->Setting_GammaCoeficient = 1.0f;
    this->Setting_VertexSphere_Visible = false;
    this->Setting_VertexSphere_Radius = 0.5f;
    this->Setting_VertexSphere_Segments = 8;
    this->Setting_VertexSphere_Color = glm::vec4(1.0);
    this->Setting_VertexSphere_ColorPickerOpen = false;
    this->Setting_VertexSphere_IsSphere = true;
    this->Setting_VertexSphere_ShowWireframes = true;
    this->VertexEditorModeID = -1;
    this->VertexEditorMode = glm::vec3(0.0);
    this->Setting_GeometryEditMode = GeometryEditMode_Vertex;
    this->Setting_Rendering_Depth = false;
    this->Setting_DebugShadowTexture = false;

    Settings::Instance()->mRayDraw = false;
    Settings::Instance()->mRayAnimate = false;
    Settings::Instance()->mRayOriginX = 0;
    Settings::Instance()->mRayOriginY = 0;
    Settings::Instance()->mRayOriginZ = 0;
    Settings::Instance()->mRayDirectionX = 0;
    Settings::Instance()->mRayDirectionY = 0;
    Settings::Instance()->mRayDirectionZ = 0;
}

void ObjectsManager::resetPropertiesSystem() {
    this->resetSettings();

    if (this->camera)
        this->camera->initProperties();
    if (this->cameraModel)
        this->cameraModel->initProperties();
    if (this->grid)
        this->grid->initProperties();
    if (this->axisSystem)
        this->axisSystem->initProperties();
    for (size_t i=0; i<this->lightSources.size(); i++) {
        this->lightSources[i]->initProperties(this->lightSources[i]->type);
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
    this->grid->initBuffers(this->Setting_GridSize, 1);
}

/*
 *
 * Camera ModelFace
 *
 */
void ObjectsManager::initCameraModel() {
    this->cameraModel = new CameraModel();
    this->cameraModel->initProperties();
    this->cameraModel->initShaderProgram();
    this->cameraModel->setModel(this->systemModels["camera"]);
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
    this->axisHelpers_xMinus->setModel(this->systemModels["axis_x_minus"]);
    this->axisHelpers_xMinus->initShaderProgram();
    this->axisHelpers_xMinus->initBuffers();

    this->axisHelpers_xPlus = new AxisHelpers();
    this->axisHelpers_xPlus->init();
    this->axisHelpers_xPlus->setModel(this->systemModels["axis_x_plus"]);
    this->axisHelpers_xPlus->initShaderProgram();
    this->axisHelpers_xPlus->initBuffers();

    this->axisHelpers_yMinus = new AxisHelpers();
    this->axisHelpers_yMinus->init();
    this->axisHelpers_yMinus->setModel(this->systemModels["axis_y_minus"]);
    this->axisHelpers_yMinus->initShaderProgram();
    this->axisHelpers_yMinus->initBuffers();

    this->axisHelpers_yPlus = new AxisHelpers();
    this->axisHelpers_yPlus->init();
    this->axisHelpers_yPlus->setModel(this->systemModels["axis_y_plus"]);
    this->axisHelpers_yPlus->initShaderProgram();
    this->axisHelpers_yPlus->initBuffers();

    this->axisHelpers_zMinus = new AxisHelpers();
    this->axisHelpers_zMinus->init();
    this->axisHelpers_zMinus->setModel(this->systemModels["axis_z_minus"]);
    this->axisHelpers_zMinus->initShaderProgram();
    this->axisHelpers_zMinus->initBuffers();

    this->axisHelpers_zPlus = new AxisHelpers();
    this->axisHelpers_zPlus->init();
    this->axisHelpers_zPlus->setModel(this->systemModels["axis_z_plus"]);
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
 * Terrain
 *
 */
void ObjectsManager::initTerrain() {
    this->terrain = new Terrain();
    this->terrain->init();
    this->terrain->initShaderProgram();
}

void ObjectsManager::generateTerrain() {
    this->terrain->initBuffers(Settings::Instance()->currentFolder, this->Setting_TerrainWidth, this->Setting_TerrainHeight);
}

/*
 *
 * Spaceship Generator
 *
 */
void ObjectsManager::initSpaceship() {
    this->spaceship = new Spaceship();
    this->spaceship->init();
    this->spaceship->initShaderProgram();
}

void ObjectsManager::generateSpaceship() {
    this->spaceship->initBuffers(this->Setting_GridSize);
}

/*
 *
 * Lights
 *
 */
void ObjectsManager::addLight(const LightSourceType type, std::string const& title, std::string const& description) {
    Light *lightObject = new Light();
    lightObject->init();
    lightObject->initProperties(type);
    lightObject->type = type;
    assert(type == LightSourceType_Directional || type == LightSourceType_Point || type == LightSourceType_Spot);
    switch (type) {
        case LightSourceType_Directional:
            lightObject->title = ((title == "") ? "Directional " + std::to_string(int(this->lightSources.size()) + 1) : title);
            lightObject->description = ((description == "") ? "Directional area light source" : description);
            lightObject->setModel(this->systemModels["light_directional"]);
            break;
        case LightSourceType_Point:
            lightObject->title = ((title == "") ? "Point " + std::to_string(int(this->lightSources.size()) + 1) : title);
            lightObject->description = ((description == "") ? "Omnidirectional point light source" : description);
            lightObject->setModel(this->systemModels["light_point"]);
            break;
        case LightSourceType_Spot:
            lightObject->title = ((title == "") ? "Spot " + std::to_string(int(this->lightSources.size()) + 1) : title);
            lightObject->description = ((description == "") ? "Directional cone light source" : description);
            lightObject->setModel(this->systemModels["light_spot"]);
            break;
    }
    lightObject->initShaderProgram();
    lightObject->initBuffers(std::string(Settings::Instance()->appFolder()));
    this->lightSources.push_back(lightObject);
}

void ObjectsManager::clearAllLights() {
    for (size_t i=0; i<this->lightSources.size(); i++) {
        Light* l = this->lightSources[i];
        delete l;
    }
    this->lightSources.clear();
}

/*
 *
 * Utilities
 *
 */
void ObjectsManager::loadSystemModels(std::unique_ptr<FileModelManager> &fileParser) {
    FBEntity file;
    file.isFile = true;
    file.extension = ".obj";

    FileBrowser_ParserType parserType = FileBrowser_ParserType_Own2;

    file.title = "light_directional";
    file.path = Settings::Instance()->appFolder() + "/gui/light_directional.obj";
    this->systemModels["light_directional"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "light_point";
    file.path = Settings::Instance()->appFolder() + "/gui/light_point.obj";
    this->systemModels["light_point"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "light_spot";
    file.path = Settings::Instance()->appFolder() + "/gui/light_spot.obj";
    this->systemModels["light_spot"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "camera";
    file.path = Settings::Instance()->appFolder() + "/gui/camera.obj";
    this->systemModels["camera"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "x_plus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/x_plus.obj";
    this->systemModels["axis_x_plus"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "x_minus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/x_minus.obj";
    this->systemModels["axis_x_minus"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "y_plus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/y_plus.obj";
    this->systemModels["axis_y_plus"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "y_minus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/y_minus.obj";
    this->systemModels["axis_y_minus"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "z_plus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/z_plus.obj";
    this->systemModels["axis_z_plus"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];

    file.title = "z_minus";
    file.path = Settings::Instance()->appFolder() + "/axis_helpers/z_minus.obj";
    this->systemModels["axis_z_minus"] = fileParser->parse(file, parserType, std::vector<std::string>())[0];
}
