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

    std::ofstream kuplungFile;
    kuplungFile.open(fileName.c_str(), std::ios::binary | std::ios::out);

    if (kuplungFile.is_open() && !kuplungFile.bad()) {
        this->storeObjectsManagerSettings(kuplungFile, managerObjects);
//        this->storeGlobalLights(kuplungFile, managerObjects);
//        this->storeObjects(kuplungFile, meshModelFaces);

        google::protobuf::ShutdownProtobufLibrary();
        kuplungFile.close();
    }
}

std::vector<ModelFaceData*> SaveOpenGProtocolBufs::openKuplungFile(FBEntity file, ObjectsManager *managerObjects) {
    std::vector<ModelFaceData*> models;

    std::ifstream kuplungFile;
    kuplungFile.open(file.path.c_str(), std::ios::binary | std::ios::in);

    if (kuplungFile.is_open() && !kuplungFile.bad()) {
        kuplungFile.seekg(0);

        this->readObjectsManagerSettings(kuplungFile, managerObjects);
//        this->readGlobalLights(kuplungFile, managerObjects);
//        models = this->readObjects(kuplungFile, managerObjects);

        google::protobuf::ShutdownProtobufLibrary();
        kuplungFile.close();
    }

    return models;
}

void SaveOpenGProtocolBufs::storeObjectsManagerSettings(std::ostream& kuplungFile, ObjectsManager *managerObjects) {
    KuplungProtoBufs::ManagerObjects bufManagerObjects;

    bufManagerObjects.set_setting_fov(managerObjects->Setting_FOV);
    bufManagerObjects.set_setting_outlinethickness(managerObjects->Setting_OutlineThickness);
    bufManagerObjects.set_setting_ratiowidth(managerObjects->Setting_RatioWidth);
    bufManagerObjects.set_setting_ratioheight(managerObjects->Setting_RatioHeight);
    bufManagerObjects.set_setting_planeclose(managerObjects->Setting_PlaneClose);
    bufManagerObjects.set_setting_planefar(managerObjects->Setting_PlaneFar);
    bufManagerObjects.set_setting_gridsize(managerObjects->Setting_GridSize);
    bufManagerObjects.set_setting_skybox(managerObjects->Setting_Skybox);
    bufManagerObjects.set_allocated_setting_outlinecolor(this->getVec4(managerObjects->Setting_OutlineColor));
    bufManagerObjects.set_allocated_setting_uiambientlight(this->getVec3(managerObjects->Setting_UIAmbientLight));
    bufManagerObjects.set_setting_fixedgridworld(managerObjects->Setting_FixedGridWorld);
    bufManagerObjects.set_setting_outlinecolorpickeropen(managerObjects->Setting_OutlineColorPickerOpen);
    bufManagerObjects.set_setting_showaxishelpers(managerObjects->Setting_ShowAxisHelpers);
    bufManagerObjects.set_settings_showzaxis(managerObjects->Settings_ShowZAxis);
    bufManagerObjects.set_viewmodelskin(managerObjects->viewModelSkin);
    bufManagerObjects.set_allocated_solidlight_direction(this->getVec3(managerObjects->SolidLight_Direction));
    bufManagerObjects.set_allocated_solidlight_materialcolor(this->getVec3(managerObjects->SolidLight_MaterialColor));
    bufManagerObjects.set_allocated_solidlight_ambient(this->getVec3(managerObjects->SolidLight_Ambient));
    bufManagerObjects.set_allocated_solidlight_diffuse(this->getVec3(managerObjects->SolidLight_Diffuse));
    bufManagerObjects.set_allocated_solidlight_specular(this->getVec3(managerObjects->SolidLight_Specular));
    bufManagerObjects.set_solidlight_ambient_strength(managerObjects->SolidLight_Ambient_Strength);
    bufManagerObjects.set_solidlight_diffuse_strength(managerObjects->SolidLight_Diffuse_Strength);
    bufManagerObjects.set_solidlight_specular_strength(managerObjects->SolidLight_Specular_Strength);
    bufManagerObjects.set_solidlight_materialcolor_colorpicker(managerObjects->SolidLight_MaterialColor_ColorPicker);
    bufManagerObjects.set_solidlight_ambient_colorpicker(managerObjects->SolidLight_Ambient_ColorPicker);
    bufManagerObjects.set_solidlight_diffuse_colorpicker(managerObjects->SolidLight_Diffuse_ColorPicker);
    bufManagerObjects.set_solidlight_specular_colorpicker(managerObjects->SolidLight_Specular_ColorPicker);
    bufManagerObjects.set_setting_showterrain(managerObjects->Setting_ShowTerrain);
    bufManagerObjects.set_setting_terrainmodel(managerObjects->Setting_TerrainModel);
    bufManagerObjects.set_setting_terrainanimatex(managerObjects->Setting_TerrainAnimateX);
    bufManagerObjects.set_setting_terrainanimatey(managerObjects->Setting_TerrainAnimateY);
    bufManagerObjects.set_heightmapimage(managerObjects->heightmapImage);
    bufManagerObjects.set_setting_terrainwidth(managerObjects->Setting_TerrainWidth);
    bufManagerObjects.set_setting_terrainheight(managerObjects->Setting_TerrainHeight);
    bufManagerObjects.set_setting_deferredtestmode(managerObjects->Setting_DeferredTestMode);
    bufManagerObjects.set_setting_deferredtestlights(managerObjects->Setting_DeferredTestLights);
    bufManagerObjects.set_setting_lightingpass_drawmode(managerObjects->Setting_LightingPass_DrawMode);
    bufManagerObjects.set_setting_deferredambientstrength(managerObjects->Setting_DeferredAmbientStrength);
    bufManagerObjects.set_setting_deferredtestlightsnumber(managerObjects->Setting_DeferredTestLightsNumber);
    bufManagerObjects.set_setting_showspaceship(managerObjects->Setting_ShowSpaceship);
    bufManagerObjects.set_setting_generatespaceship(managerObjects->Setting_GenerateSpaceship);

//    bufManagerObjects.set_allocated_matrixprojection(this->getMatrix(managerObjects->matrixProjection));

    ::KuplungProtoBufs::CameraSettings* bufCamera = new ::KuplungProtoBufs::CameraSettings();
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
    bufManagerObjects.set_allocated_camera(bufCamera);

    ::KuplungProtoBufs::GridSettings* bufGrid = new ::KuplungProtoBufs::GridSettings();
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
    bufManagerObjects.set_allocated_grid(bufGrid);

    if (!bufManagerObjects.SerializeToOstream(&kuplungFile))
      printf("Failed to write managerObjects settings.\n");
}

void SaveOpenGProtocolBufs::readObjectsManagerSettings(std::istream& kuplungFile, ObjectsManager *managerObjects) {
    KuplungProtoBufs::ManagerObjects bufManagerObjects;
    if (!bufManagerObjects.ParseFromIstream(&kuplungFile))
        printf("Failed to read managerObjects settings.\n");
    else {
        managerObjects->Setting_FOV = bufManagerObjects.setting_fov();
        managerObjects->Setting_OutlineThickness = bufManagerObjects.setting_outlinethickness();
        managerObjects->Setting_RatioWidth = bufManagerObjects.setting_ratiowidth();
        managerObjects->Setting_RatioHeight = bufManagerObjects.setting_ratioheight();
        managerObjects->Setting_PlaneClose = bufManagerObjects.setting_planeclose();
        managerObjects->Setting_PlaneFar = bufManagerObjects.setting_planefar();
        managerObjects->Setting_GridSize = bufManagerObjects.setting_gridsize();
        managerObjects->Setting_Skybox = bufManagerObjects.setting_skybox();
        managerObjects->Setting_OutlineColor = this->setVec4(bufManagerObjects.setting_outlinecolor());
        managerObjects->Setting_UIAmbientLight = this->setVec3(bufManagerObjects.setting_uiambientlight());
        managerObjects->Setting_FixedGridWorld = bufManagerObjects.setting_fixedgridworld();
        managerObjects->Setting_OutlineColorPickerOpen = bufManagerObjects.setting_outlinecolorpickeropen();
        managerObjects->Setting_ShowAxisHelpers = bufManagerObjects.setting_showaxishelpers();
        managerObjects->Settings_ShowZAxis = bufManagerObjects.settings_showzaxis();
        switch (bufManagerObjects.viewmodelskin()) {
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
        managerObjects->SolidLight_Direction = this->setVec3(bufManagerObjects.solidlight_direction());
        managerObjects->SolidLight_MaterialColor = this->setVec3(bufManagerObjects.solidlight_materialcolor());
        managerObjects->SolidLight_Ambient = this->setVec3(bufManagerObjects.solidlight_ambient());
        managerObjects->SolidLight_Diffuse = this->setVec3(bufManagerObjects.solidlight_diffuse());
        managerObjects->SolidLight_Specular = this->setVec3(bufManagerObjects.solidlight_specular());
        managerObjects->SolidLight_Ambient_Strength = bufManagerObjects.solidlight_ambient_strength();
        managerObjects->SolidLight_Diffuse_Strength = bufManagerObjects.solidlight_diffuse_strength();
        managerObjects->SolidLight_Specular_Strength = bufManagerObjects.solidlight_specular_strength();
        managerObjects->SolidLight_MaterialColor_ColorPicker = bufManagerObjects.solidlight_materialcolor_colorpicker();
        managerObjects->SolidLight_Ambient_ColorPicker = bufManagerObjects.solidlight_ambient_colorpicker();
        managerObjects->SolidLight_Diffuse_ColorPicker = bufManagerObjects.solidlight_diffuse_colorpicker();
        managerObjects->SolidLight_Specular_ColorPicker = bufManagerObjects.solidlight_specular_colorpicker();
        managerObjects->Setting_ShowTerrain = bufManagerObjects.setting_showterrain();
        managerObjects->Setting_TerrainModel = bufManagerObjects.setting_terrainmodel();
        managerObjects->Setting_TerrainAnimateX = bufManagerObjects.setting_terrainanimatex();
        managerObjects->Setting_TerrainAnimateY = bufManagerObjects.setting_terrainanimatey();
        managerObjects->heightmapImage = bufManagerObjects.heightmapimage();
        managerObjects->Setting_TerrainWidth = bufManagerObjects.setting_terrainwidth();
        managerObjects->Setting_TerrainHeight = bufManagerObjects.setting_terrainheight();
        managerObjects->Setting_DeferredTestMode = bufManagerObjects.setting_deferredtestmode();
        managerObjects->Setting_DeferredTestLights = bufManagerObjects.setting_deferredtestlights();
        managerObjects->Setting_LightingPass_DrawMode = bufManagerObjects.setting_lightingpass_drawmode();
        managerObjects->Setting_DeferredAmbientStrength = bufManagerObjects.setting_deferredambientstrength();
        managerObjects->Setting_DeferredTestLightsNumber = bufManagerObjects.setting_deferredtestlightsnumber();
        managerObjects->Setting_ShowSpaceship = bufManagerObjects.setting_showspaceship();
        managerObjects->Setting_GenerateSpaceship = bufManagerObjects.setting_generatespaceship();

//        managerObjects->matrixProjection = this->setMatrix(bufManagerObjects.matrixprojection());

        const ::KuplungProtoBufs::CameraSettings& camera = bufManagerObjects.camera();
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

        const ::KuplungProtoBufs::GridSettings& grid = bufManagerObjects.grid();
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
}

void SaveOpenGProtocolBufs::storeGlobalLights(std::ostream& kuplungFile, ObjectsManager *managerObjects) {
    KuplungProtoBufs::ManagerObjects bufManagerObjects;

    for (size_t i=0; i<managerObjects->lightSources.size(); i++) {
        Light* l = managerObjects->lightSources[i];
        KuplungProtoBufs::LightObject* lo = bufManagerObjects.add_lights();
        lo->set_title(l->title);
        lo->set_description(l->description);
        lo->set_type(l->type);
    }

    if (!bufManagerObjects.SerializeToOstream(&kuplungFile))
      printf("Failed to write Lights.\n");
}

void SaveOpenGProtocolBufs::readGlobalLights(std::istream& kuplungFile, ObjectsManager *managerObjects) {
    KuplungProtoBufs::ManagerObjects bufManagerObjects;
    if (!bufManagerObjects.ParseFromIstream(&kuplungFile))
        printf("Failed to read Lights.\n");
    else {
        for (int i=0; i<bufManagerObjects.lights_size(); i++) {
            KuplungProtoBufs::LightObject lo = bufManagerObjects.lights(i);

            Light* l = new Light();
            l->init(LightSourceType_Directional);
            l->initProperties();
            l->title = lo.title();
            l->description = lo.description();

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

            printf("%s = %s\n", l->title.c_str(), l->description.c_str());
        }
    }
}

void SaveOpenGProtocolBufs::storeObjects(std::ostream& kuplungFile, std::vector<ModelFaceBase*> meshModelFaces) {
}

std::vector<ModelFaceData*> SaveOpenGProtocolBufs::readObjects(std::istream& kuplungFile, ObjectsManager *managerObjects) {
    std::vector<ModelFaceData*> models;
    return models;
}

::KuplungProtoBufs::Vec4* SaveOpenGProtocolBufs::getVec4(glm::vec4 v) {
    ::KuplungProtoBufs::Vec4* gv = new ::KuplungProtoBufs::Vec4();
    gv->set_x(v.x);
    gv->set_y(v.y);
    gv->set_z(v.z);
    gv->set_w(v.w);
    return gv;
}

::KuplungProtoBufs::Vec3* SaveOpenGProtocolBufs::getVec3(glm::vec3 v) {
    ::KuplungProtoBufs::Vec3* gv = new ::KuplungProtoBufs::Vec3();
    gv->set_x(v.x);
    gv->set_y(v.y);
    gv->set_z(v.z);
    return gv;
}

::KuplungProtoBufs::Vec2* SaveOpenGProtocolBufs::getVec2(glm::vec2 v) {
    ::KuplungProtoBufs::Vec2* gv = new ::KuplungProtoBufs::Vec2();
    gv->set_x(v.x);
    gv->set_y(v.y);
    return gv;
}

glm::vec4 SaveOpenGProtocolBufs::setVec4(const ::KuplungProtoBufs::Vec4& v) {
    return glm::vec4(v.x(), v.y(), v.z(), v.w());
}

glm::vec3 SaveOpenGProtocolBufs::setVec3(const ::KuplungProtoBufs::Vec3& v) {
    return glm::vec3(v.x(), v.y(), v.z());
}

glm::vec2 SaveOpenGProtocolBufs::setVec2(const ::KuplungProtoBufs::Vec2& v) {
    return glm::vec2(v.x(), v.y());
}

//::KuplungProtoBufs::FloatMatrix* SaveOpenGProtocolBufs::getMatrix(glm::mat4 m) {
//    ::KuplungProtoBufs::FloatMatrix* mtx = new ::KuplungProtoBufs::FloatMatrix();
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

//glm::mat4 SaveOpenGProtocolBufs::setMatrix(const ::KuplungProtoBufs::FloatMatrix& m) {
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

::KuplungProtoBufs::ObjectCoordinate* SaveOpenGProtocolBufs::getObjectCoordinate(ObjectCoordinate* v) {
    ::KuplungProtoBufs::ObjectCoordinate* oc = new ::KuplungProtoBufs::ObjectCoordinate();
    oc->set_animate(v->animate);
    oc->set_point(v->point);
    return oc;
}

ObjectCoordinate* SaveOpenGProtocolBufs::setObjectCoordinate(const ::KuplungProtoBufs::ObjectCoordinate& v) {
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
