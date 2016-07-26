//
//  SaveOpenGProtocolBufs.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SaveOpenGProtocolBufs.hpp"
#include <iostream>
#include <fstream>
#include <glm/gtx/string_cast.hpp>

void SaveOpenGProtocolBufs::init() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

void SaveOpenGProtocolBufs::saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces) {
    std::string fileName = file.path;
    if (!this->hasEnding(fileName, ".kuplung"))
        fileName += ".kuplung";
    std::remove(fileName.c_str());

    std::string fileNameSettings = fileName + ".settings";
    std::string fileNameScene = fileName + ".scene";

    std::ofstream kuplungFileSettings;
    kuplungFileSettings.open(fileNameSettings.c_str(), std::ios::binary | std::ios::out);
    if (kuplungFileSettings.is_open() && !kuplungFileSettings.bad()) {
        this->storeObjectsManagerSettings(managerObjects);
        this->storeGlobalLights(managerObjects);

        if (!this->bufGUISettings.SerializeToOstream(&kuplungFileSettings))
          printf("Failed to write App Settings.\n");

        google::protobuf::ShutdownProtobufLibrary();
        kuplungFileSettings.close();
    }

    std::ofstream kuplungFileScene;
    kuplungFileScene.open(fileNameScene.c_str(), std::ios::binary | std::ios::out);
    if (kuplungFileScene.is_open() && !kuplungFileScene.bad()) {
        this->storeObjects(meshModelFaces);

        if (!this->bufScene.SerializeToOstream(&kuplungFileScene))
          printf("Failed to write Scene.\n");

        google::protobuf::ShutdownProtobufLibrary();
        kuplungFileScene.close();
    }
}

std::vector<ModelFaceData*> SaveOpenGProtocolBufs::openKuplungFile(FBEntity file, ObjectsManager *managerObjects) {
    std::vector<ModelFaceData*> models;

    std::string fileNameSettings = file.path + ".settings";
    std::string fileNameScene = file.path + ".scene";

    std::ifstream kuplungFileSettings;
    kuplungFileSettings.open(fileNameSettings.c_str(), std::ios::binary | std::ios::in);
    if (kuplungFileSettings.is_open() && !kuplungFileSettings.bad()) {
        kuplungFileSettings.seekg(0);

        if (!this->bufGUISettings.ParseFromIstream(&kuplungFileSettings))
            printf("Failed to read App Settings.\n");
        else {
            this->readObjectsManagerSettings(managerObjects);
            this->readGlobalLights(managerObjects);
        }

        google::protobuf::ShutdownProtobufLibrary();
        kuplungFileSettings.close();
    }

    std::ifstream kuplungFileScene;
    kuplungFileScene.open(fileNameScene.c_str(), std::ios::binary | std::ios::in);
    if (kuplungFileScene.is_open() && !kuplungFileScene.bad()) {
        kuplungFileScene.seekg(0);

        if (!this->bufScene.ParseFromIstream(&kuplungFileScene))
            printf("Failed to read Scene.\n");
        else
            models = this->readObjects(managerObjects);

        google::protobuf::ShutdownProtobufLibrary();
        kuplungFileScene.close();
    }

    return models;
}

void SaveOpenGProtocolBufs::storeObjectsManagerSettings(ObjectsManager *managerObjects) {
    this->bufGUISettings.set_setting_fov(managerObjects->Setting_FOV);
    this->bufGUISettings.set_setting_outlinethickness(managerObjects->Setting_OutlineThickness);
    this->bufGUISettings.set_setting_ratiowidth(managerObjects->Setting_RatioWidth);
    this->bufGUISettings.set_setting_ratioheight(managerObjects->Setting_RatioHeight);
    this->bufGUISettings.set_setting_planeclose(managerObjects->Setting_PlaneClose);
    this->bufGUISettings.set_setting_planefar(managerObjects->Setting_PlaneFar);
    this->bufGUISettings.set_setting_gridsize(managerObjects->Setting_GridSize);
    this->bufGUISettings.set_setting_skybox(managerObjects->Setting_Skybox);
    this->bufGUISettings.set_allocated_setting_outlinecolor(this->getVec4(managerObjects->Setting_OutlineColor));
    this->bufGUISettings.set_allocated_setting_uiambientlight(this->getVec3(managerObjects->Setting_UIAmbientLight));
    this->bufGUISettings.set_setting_fixedgridworld(managerObjects->Setting_FixedGridWorld);
    this->bufGUISettings.set_setting_outlinecolorpickeropen(managerObjects->Setting_OutlineColorPickerOpen);
    this->bufGUISettings.set_setting_showaxishelpers(managerObjects->Setting_ShowAxisHelpers);
    this->bufGUISettings.set_settings_showzaxis(managerObjects->Settings_ShowZAxis);
    this->bufGUISettings.set_viewmodelskin(managerObjects->viewModelSkin);
    this->bufGUISettings.set_allocated_solidlight_direction(this->getVec3(managerObjects->SolidLight_Direction));
    this->bufGUISettings.set_allocated_solidlight_materialcolor(this->getVec3(managerObjects->SolidLight_MaterialColor));
    this->bufGUISettings.set_allocated_solidlight_ambient(this->getVec3(managerObjects->SolidLight_Ambient));
    this->bufGUISettings.set_allocated_solidlight_diffuse(this->getVec3(managerObjects->SolidLight_Diffuse));
    this->bufGUISettings.set_allocated_solidlight_specular(this->getVec3(managerObjects->SolidLight_Specular));
    this->bufGUISettings.set_solidlight_ambient_strength(managerObjects->SolidLight_Ambient_Strength);
    this->bufGUISettings.set_solidlight_diffuse_strength(managerObjects->SolidLight_Diffuse_Strength);
    this->bufGUISettings.set_solidlight_specular_strength(managerObjects->SolidLight_Specular_Strength);
    this->bufGUISettings.set_solidlight_materialcolor_colorpicker(managerObjects->SolidLight_MaterialColor_ColorPicker);
    this->bufGUISettings.set_solidlight_ambient_colorpicker(managerObjects->SolidLight_Ambient_ColorPicker);
    this->bufGUISettings.set_solidlight_diffuse_colorpicker(managerObjects->SolidLight_Diffuse_ColorPicker);
    this->bufGUISettings.set_solidlight_specular_colorpicker(managerObjects->SolidLight_Specular_ColorPicker);
    this->bufGUISettings.set_setting_showterrain(managerObjects->Setting_ShowTerrain);
    this->bufGUISettings.set_setting_terrainmodel(managerObjects->Setting_TerrainModel);
    this->bufGUISettings.set_setting_terrainanimatex(managerObjects->Setting_TerrainAnimateX);
    this->bufGUISettings.set_setting_terrainanimatey(managerObjects->Setting_TerrainAnimateY);
    this->bufGUISettings.set_heightmapimage(managerObjects->heightmapImage);
    this->bufGUISettings.set_setting_terrainwidth(managerObjects->Setting_TerrainWidth);
    this->bufGUISettings.set_setting_terrainheight(managerObjects->Setting_TerrainHeight);
    this->bufGUISettings.set_setting_deferredtestmode(managerObjects->Setting_DeferredTestMode);
    this->bufGUISettings.set_setting_deferredtestlights(managerObjects->Setting_DeferredTestLights);
    this->bufGUISettings.set_setting_lightingpass_drawmode(managerObjects->Setting_LightingPass_DrawMode);
    this->bufGUISettings.set_setting_deferredambientstrength(managerObjects->Setting_DeferredAmbientStrength);
    this->bufGUISettings.set_setting_deferredtestlightsnumber(managerObjects->Setting_DeferredTestLightsNumber);
    this->bufGUISettings.set_setting_showspaceship(managerObjects->Setting_ShowSpaceship);
    this->bufGUISettings.set_setting_generatespaceship(managerObjects->Setting_GenerateSpaceship);

//    this->bufGUISettings.set_allocated_matrixprojection(this->getMatrix(managerObjects->matrixProjection));

    KuplungApp::CameraSettings* bufCamera = new KuplungApp::CameraSettings();
    bufCamera->set_allocated_cameraposition(this->getVec3(managerObjects->camera->cameraPosition));
    bufCamera->set_allocated_view_eye(this->getVec3(managerObjects->camera->eyeSettings->View_Eye));
    bufCamera->set_allocated_view_center(this->getVec3(managerObjects->camera->eyeSettings->View_Center));
    bufCamera->set_allocated_view_up(this->getVec3(managerObjects->camera->eyeSettings->View_Up));
    bufCamera->set_allocated_positionx(this->getObjectCoordinate(managerObjects->camera->positionX));
    bufCamera->set_allocated_positiony(this->getObjectCoordinate(managerObjects->camera->positionY));
    bufCamera->set_allocated_positionz(this->getObjectCoordinate(managerObjects->camera->positionZ));
    bufCamera->set_allocated_rotatex(this->getObjectCoordinate(managerObjects->camera->rotateX));
    bufCamera->set_allocated_rotatey(this->getObjectCoordinate(managerObjects->camera->rotateY));
    bufCamera->set_allocated_rotatez(this->getObjectCoordinate(managerObjects->camera->rotateZ));
    bufCamera->set_allocated_rotatecenterx(this->getObjectCoordinate(managerObjects->camera->rotateCenterX));
    bufCamera->set_allocated_rotatecentery(this->getObjectCoordinate(managerObjects->camera->rotateCenterY));
    bufCamera->set_allocated_rotatecenterz(this->getObjectCoordinate(managerObjects->camera->rotateCenterZ));
    this->bufGUISettings.set_allocated_camera(bufCamera);

    KuplungApp::GridSettings* bufGrid = new KuplungApp::GridSettings();
    bufGrid->set_actasmirror(managerObjects->grid->actAsMirror);
    bufGrid->set_gridsize(managerObjects->grid->gridSize);
    bufGrid->set_allocated_positionx(this->getObjectCoordinate(managerObjects->grid->positionX));
    bufGrid->set_allocated_positiony(this->getObjectCoordinate(managerObjects->grid->positionY));
    bufGrid->set_allocated_positionz(this->getObjectCoordinate(managerObjects->grid->positionZ));
    bufGrid->set_allocated_rotatex(this->getObjectCoordinate(managerObjects->grid->rotateX));
    bufGrid->set_allocated_rotatey(this->getObjectCoordinate(managerObjects->grid->rotateY));
    bufGrid->set_allocated_rotatez(this->getObjectCoordinate(managerObjects->grid->rotateZ));
    bufGrid->set_allocated_scalex(this->getObjectCoordinate(managerObjects->grid->scaleX));
    bufGrid->set_allocated_scaley(this->getObjectCoordinate(managerObjects->grid->scaleY));
    bufGrid->set_allocated_scalez(this->getObjectCoordinate(managerObjects->grid->scaleZ));
    bufGrid->set_transparency(managerObjects->grid->transparency);
    bufGrid->set_showgrid(managerObjects->grid->showGrid);
    this->bufGUISettings.set_allocated_grid(bufGrid);
}

void SaveOpenGProtocolBufs::readObjectsManagerSettings(ObjectsManager *managerObjects) {
    managerObjects->Setting_FOV = this->bufGUISettings.setting_fov();
    managerObjects->Setting_OutlineThickness = this->bufGUISettings.setting_outlinethickness();
    managerObjects->Setting_RatioWidth = this->bufGUISettings.setting_ratiowidth();
    managerObjects->Setting_RatioHeight = this->bufGUISettings.setting_ratioheight();
    managerObjects->Setting_PlaneClose = this->bufGUISettings.setting_planeclose();
    managerObjects->Setting_PlaneFar = this->bufGUISettings.setting_planefar();
    managerObjects->Setting_GridSize = this->bufGUISettings.setting_gridsize();
    managerObjects->Setting_Skybox = this->bufGUISettings.setting_skybox();
    managerObjects->Setting_OutlineColor = this->setVec4(this->bufGUISettings.setting_outlinecolor());
    managerObjects->Setting_UIAmbientLight = this->setVec3(this->bufGUISettings.setting_uiambientlight());
    managerObjects->Setting_FixedGridWorld = this->bufGUISettings.setting_fixedgridworld();
    managerObjects->Setting_OutlineColorPickerOpen = this->bufGUISettings.setting_outlinecolorpickeropen();
    managerObjects->Setting_ShowAxisHelpers = this->bufGUISettings.setting_showaxishelpers();
    managerObjects->Settings_ShowZAxis = this->bufGUISettings.settings_showzaxis();
    switch (this->bufGUISettings.viewmodelskin()) {
        case 0:
            managerObjects->viewModelSkin = ViewModelSkin_Solid;
            break;
        case 1:
            managerObjects->viewModelSkin = ViewModelSkin_Material;
            break;
        case 2:
            managerObjects->viewModelSkin = ViewModelSkin_Texture;
            break;
        case 3:
            managerObjects->viewModelSkin = ViewModelSkin_Wireframe;
            break;
        case 4:
            managerObjects->viewModelSkin = ViewModelSkin_Rendered;
            break;
        default:
            break;
    }
    managerObjects->SolidLight_Direction = this->setVec3(this->bufGUISettings.solidlight_direction());
    managerObjects->SolidLight_MaterialColor = this->setVec3(this->bufGUISettings.solidlight_materialcolor());
    managerObjects->SolidLight_Ambient = this->setVec3(this->bufGUISettings.solidlight_ambient());
    managerObjects->SolidLight_Diffuse = this->setVec3(this->bufGUISettings.solidlight_diffuse());
    managerObjects->SolidLight_Specular = this->setVec3(this->bufGUISettings.solidlight_specular());
    managerObjects->SolidLight_Ambient_Strength = this->bufGUISettings.solidlight_ambient_strength();
    managerObjects->SolidLight_Diffuse_Strength = this->bufGUISettings.solidlight_diffuse_strength();
    managerObjects->SolidLight_Specular_Strength = this->bufGUISettings.solidlight_specular_strength();
    managerObjects->SolidLight_MaterialColor_ColorPicker = this->bufGUISettings.solidlight_materialcolor_colorpicker();
    managerObjects->SolidLight_Ambient_ColorPicker = this->bufGUISettings.solidlight_ambient_colorpicker();
    managerObjects->SolidLight_Diffuse_ColorPicker = this->bufGUISettings.solidlight_diffuse_colorpicker();
    managerObjects->SolidLight_Specular_ColorPicker = this->bufGUISettings.solidlight_specular_colorpicker();
    managerObjects->Setting_ShowTerrain = this->bufGUISettings.setting_showterrain();
    managerObjects->Setting_TerrainModel = this->bufGUISettings.setting_terrainmodel();
    managerObjects->Setting_TerrainAnimateX = this->bufGUISettings.setting_terrainanimatex();
    managerObjects->Setting_TerrainAnimateY = this->bufGUISettings.setting_terrainanimatey();
    managerObjects->heightmapImage = this->bufGUISettings.heightmapimage();
    managerObjects->Setting_TerrainWidth = this->bufGUISettings.setting_terrainwidth();
    managerObjects->Setting_TerrainHeight = this->bufGUISettings.setting_terrainheight();
    managerObjects->Setting_DeferredTestMode = this->bufGUISettings.setting_deferredtestmode();
    managerObjects->Setting_DeferredTestLights = this->bufGUISettings.setting_deferredtestlights();
    managerObjects->Setting_LightingPass_DrawMode = this->bufGUISettings.setting_lightingpass_drawmode();
    managerObjects->Setting_DeferredAmbientStrength = this->bufGUISettings.setting_deferredambientstrength();
    managerObjects->Setting_DeferredTestLightsNumber = this->bufGUISettings.setting_deferredtestlightsnumber();
    managerObjects->Setting_ShowSpaceship = this->bufGUISettings.setting_showspaceship();
    managerObjects->Setting_GenerateSpaceship = this->bufGUISettings.setting_generatespaceship();

//        managerObjects->matrixProjection = this->setMatrix(this->bufGUISettings.matrixprojection());

    const KuplungApp::CameraSettings& camera = this->bufGUISettings.camera();
    managerObjects->camera->cameraPosition = this->setVec3(camera.cameraposition());
    managerObjects->camera->eyeSettings->View_Eye = this->setVec3(camera.view_eye());
    managerObjects->camera->eyeSettings->View_Center = this->setVec3(camera.view_center());
    managerObjects->camera->eyeSettings->View_Up = this->setVec3(camera.view_up());
    managerObjects->camera->positionX = this->setObjectCoordinate(camera.positionx());
    managerObjects->camera->positionY = this->setObjectCoordinate(camera.positiony());
    managerObjects->camera->positionZ = this->setObjectCoordinate(camera.positionz());
    managerObjects->camera->rotateX = this->setObjectCoordinate(camera.rotatex());
    managerObjects->camera->rotateY = this->setObjectCoordinate(camera.rotatey());
    managerObjects->camera->rotateZ = this->setObjectCoordinate(camera.rotatez());
    managerObjects->camera->rotateCenterX = this->setObjectCoordinate(camera.rotatecenterx());
    managerObjects->camera->rotateCenterY = this->setObjectCoordinate(camera.rotatecentery());
    managerObjects->camera->rotateCenterZ = this->setObjectCoordinate(camera.rotatecenterz());

    const KuplungApp::GridSettings& grid = this->bufGUISettings.grid();
    managerObjects->grid->actAsMirror = grid.actasmirror();
    managerObjects->grid->gridSize = grid.gridsize();
    managerObjects->grid->positionX = this->setObjectCoordinate(grid.positionx());
    managerObjects->grid->positionY = this->setObjectCoordinate(grid.positiony());
    managerObjects->grid->positionZ = this->setObjectCoordinate(grid.positionz());
    managerObjects->grid->rotateX = this->setObjectCoordinate(grid.rotatex());
    managerObjects->grid->rotateY = this->setObjectCoordinate(grid.rotatey());
    managerObjects->grid->rotateZ = this->setObjectCoordinate(grid.rotatez());
    managerObjects->grid->scaleX = this->setObjectCoordinate(grid.scalex());
    managerObjects->grid->scaleY = this->setObjectCoordinate(grid.scaley());
    managerObjects->grid->scaleZ = this->setObjectCoordinate(grid.scalez());
    managerObjects->grid->transparency = grid.transparency();
    managerObjects->grid->showGrid = grid.showgrid();
}

void SaveOpenGProtocolBufs::storeGlobalLights(ObjectsManager *managerObjects) {
    for (size_t i=0; i<managerObjects->lightSources.size(); i++) {
        Light* l = managerObjects->lightSources[i];
        KuplungApp::LightObject* lo = this->bufGUISettings.add_lights();
        lo->set_title(l->title);
        lo->set_description(l->description);
        lo->set_type(l->type);
        lo->set_showlampobject(l->showLampObject);
        lo->set_showlampdirection(l->showLampDirection);
        lo->set_showinwire(l->showInWire);
        lo->set_allocated_positionx(this->getObjectCoordinate(l->positionX));
        lo->set_allocated_positiony(this->getObjectCoordinate(l->positionY));
        lo->set_allocated_positionz(this->getObjectCoordinate(l->positionZ));
        lo->set_allocated_directionx(this->getObjectCoordinate(l->directionX));
        lo->set_allocated_directiony(this->getObjectCoordinate(l->directionY));
        lo->set_allocated_directionz(this->getObjectCoordinate(l->directionZ));
        lo->set_allocated_scalex(this->getObjectCoordinate(l->scaleX));
        lo->set_allocated_scaley(this->getObjectCoordinate(l->scaleY));
        lo->set_allocated_scalez(this->getObjectCoordinate(l->scaleZ));
        lo->set_allocated_rotatex(this->getObjectCoordinate(l->rotateX));
        lo->set_allocated_rotatey(this->getObjectCoordinate(l->rotateY));
        lo->set_allocated_rotatez(this->getObjectCoordinate(l->rotateZ));
        lo->set_allocated_rotatecenterx(this->getObjectCoordinate(l->rotateCenterX));
        lo->set_allocated_rotatecentery(this->getObjectCoordinate(l->rotateCenterY));
        lo->set_allocated_rotatecenterz(this->getObjectCoordinate(l->rotateCenterZ));
    }
}

void SaveOpenGProtocolBufs::readGlobalLights(ObjectsManager *managerObjects) {
    for (int i=0; i<this->bufGUISettings.lights_size(); i++) {
        KuplungApp::LightObject lo = this->bufGUISettings.lights(i);

        Light* l = new Light();
        l->init(LightSourceType_Directional);
        l->initProperties();
        l->title = lo.title();
        l->description = lo.description();
        l->showLampObject = lo.showlampobject();
        l->showLampDirection = lo.showlampdirection();
        l->showInWire = lo.showinwire();
        l->positionX = this->setObjectCoordinate(lo.positionx());
        l->positionY = this->setObjectCoordinate(lo.positiony());
        l->positionZ = this->setObjectCoordinate(lo.positionz());
        l->directionX = this->setObjectCoordinate(lo.directionx());
        l->directionY = this->setObjectCoordinate(lo.directiony());
        l->directionZ = this->setObjectCoordinate(lo.directionz());
        l->scaleX = this->setObjectCoordinate(lo.scalex());
        l->scaleY = this->setObjectCoordinate(lo.scaley());
        l->scaleZ = this->setObjectCoordinate(lo.scalez());
        l->rotateX = this->setObjectCoordinate(lo.rotatex());
        l->rotateY = this->setObjectCoordinate(lo.rotatey());
        l->rotateZ = this->setObjectCoordinate(lo.rotatez());
        l->rotateCenterX = this->setObjectCoordinate(lo.rotatecenterx());
        l->rotateCenterY = this->setObjectCoordinate(lo.rotatecentery());
        l->rotateCenterZ = this->setObjectCoordinate(lo.rotatecenterz());

        switch (lo.type()) {
            case 0:
                l->type = LightSourceType_Directional;
                break;
            case 1:
                l->type = LightSourceType_Point;
                break;
            case 2:
                l->type = LightSourceType_Spot;
                break;
        }

        switch (l->type) {
            case LightSourceType_Directional:
                l->setModel(managerObjects->systemModels["light_directional"]);
                break;
            case LightSourceType_Point:
                l->setModel(managerObjects->systemModels["light_point"]);
                break;
            case LightSourceType_Spot:
                l->setModel(managerObjects->systemModels["light_spot"]);
        }

        l->initShaderProgram();
        l->initBuffers(Settings::Instance()->appFolder());

        managerObjects->lightSources.push_back(l);
    }
}

void SaveOpenGProtocolBufs::storeObjects(std::vector<ModelFaceBase*> meshModelFaces) {
}

std::vector<ModelFaceData*> SaveOpenGProtocolBufs::readObjects(ObjectsManager *managerObjects) {
    std::vector<ModelFaceData*> models;
    return models;
}

KuplungApp::Vec4* SaveOpenGProtocolBufs::getVec4(glm::vec4 v) {
    KuplungApp::Vec4* gv = new KuplungApp::Vec4();
    gv->set_x(v.x);
    gv->set_y(v.y);
    gv->set_z(v.z);
    gv->set_w(v.w);
    return gv;
}

KuplungApp::Vec3* SaveOpenGProtocolBufs::getVec3(glm::vec3 v) {
    KuplungApp::Vec3* gv = new KuplungApp::Vec3();
    gv->set_x(v.x);
    gv->set_y(v.y);
    gv->set_z(v.z);
    return gv;
}

KuplungApp::Vec2* SaveOpenGProtocolBufs::getVec2(glm::vec2 v) {
    KuplungApp::Vec2* gv = new KuplungApp::Vec2();
    gv->set_x(v.x);
    gv->set_y(v.y);
    return gv;
}

glm::vec4 SaveOpenGProtocolBufs::setVec4(const KuplungApp::Vec4& v) {
    return glm::vec4(v.x(), v.y(), v.z(), v.w());
}

glm::vec3 SaveOpenGProtocolBufs::setVec3(const KuplungApp::Vec3& v) {
    return glm::vec3(v.x(), v.y(), v.z());
}

glm::vec2 SaveOpenGProtocolBufs::setVec2(const KuplungApp::Vec2& v) {
    return glm::vec2(v.x(), v.y());
}

//KuplungApp::FloatMatrix* SaveOpenGProtocolBufs::getMatrix(glm::mat4 m) {
//    KuplungApp::FloatMatrix* mtx = new KuplungApp::FloatMatrix();
//    mtx->set_rows(4);
//    mtx->set_cols(4);
//    for (int i=0; i<4; i++) {
//        for (int j=0; j<4; j++) {
//            mtx->add_data(m[i][j]);
//            printf("adding [%i][%i] = %f\n", i, j, m[i][j]);
//        }
//    }
//    return mtx;
//}

//glm::mat4 SaveOpenGProtocolBufs::setMatrix(const KuplungApp::FloatMatrix& m) {
//    glm::mat4 mtx = glm::mat4(1.0);
//    int rows = int(m.rows());
//    int columns = int(m.cols());
//    for (int i=0; i<rows; i++) {
//        for (int j=0; j<columns; j++) {
//            mtx[i][j] = m.data(i * columns + j);
//        }
//    }
//    return mtx;
//}

KuplungApp::ObjectCoordinate* SaveOpenGProtocolBufs::getObjectCoordinate(ObjectCoordinate* v) {
    KuplungApp::ObjectCoordinate* oc = new KuplungApp::ObjectCoordinate();
    oc->set_animate(v->animate);
    oc->set_point(v->point);
    return oc;
}

ObjectCoordinate* SaveOpenGProtocolBufs::setObjectCoordinate(const KuplungApp::ObjectCoordinate& v) {
    ObjectCoordinate* oc = new ObjectCoordinate();
    oc->animate = v.animate();
    oc->point = v.point();
    return oc;
}

bool SaveOpenGProtocolBufs::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}
