//
//  SaveOpenGProtocolBufs.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SaveOpenGProtocolBufs.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

SaveOpenGProtocolBufs::~SaveOpenGProtocolBufs() {
  this->managerZip.reset();
}

void SaveOpenGProtocolBufs::init() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  this->managerZip = std::make_unique<KuplungApp::Utilities::Miniz::KuplungMiniz>();
}

void SaveOpenGProtocolBufs::saveKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager>& managerObjects, const std::vector<ModelFaceBase*>& meshModelFaces) {
  std::string fileName = file.path;
  if (!this->hasEnding(fileName, ".kuplung"))
    fileName += ".kuplung";
  std::remove(fileName.c_str());

  std::string fileNameSettings = fileName + ".settings";
  std::string fileNameScene = fileName + ".scene";
  std::string fileNameZipSettings = file.title + ".settings";
  std::string fileNameZipScene = file.title + ".scene";
  if (!this->hasEnding(file.title, ".kuplung")) {
    fileNameZipSettings = file.title + ".kuplung.settings";
    fileNameZipScene = file.title + ".kuplung.scene";
  }

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

  this->managerZip->createZipFile(fileName.c_str());
  this->managerZip->addFileToArchive(fileNameSettings.c_str(), fileNameZipSettings.c_str());
  this->managerZip->addFileToArchive(fileNameScene.c_str(), fileNameZipScene.c_str());
  this->managerZip->closeZipFile();

  std::filesystem::remove(fileNameSettings.c_str());
  std::filesystem::remove(fileNameScene.c_str());
}

std::vector<ModelFaceData*> SaveOpenGProtocolBufs::openKuplungFile(const FBEntity& file, std::unique_ptr<ObjectsManager>& managerObjects) {
  std::vector<ModelFaceData*> models;

  std::string zPath = file.path;
  boost::replace_all(zPath, file.title, "");
  if (this->managerZip->unzipArchive(file.title, zPath)) {
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

      kuplungFileScene.close();
    }

    std::filesystem::remove(fileNameSettings.c_str());
    std::filesystem::remove(fileNameScene.c_str());
  }
  else
    Settings::Instance()->funcDoLog("[KuplungSave Protobuf] Cannot unzip .kuplung file!");

  google::protobuf::ShutdownProtobufLibrary();

  return models;
}

void SaveOpenGProtocolBufs::storeObjectsManagerSettings(std::unique_ptr<ObjectsManager>& managerObjects) {
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

  KuplungApp::CameraSettings* bufCamera = new KuplungApp::CameraSettings();
  bufCamera->set_allocated_cameraposition(this->getVec3(managerObjects->camera->cameraPosition));
  bufCamera->set_allocated_view_eye(this->getVec3(managerObjects->camera->eyeSettings->View_Eye));
  bufCamera->set_allocated_view_center(this->getVec3(managerObjects->camera->eyeSettings->View_Center));
  bufCamera->set_allocated_view_up(this->getVec3(managerObjects->camera->eyeSettings->View_Up));
  bufCamera->set_allocated_positionx(this->getObjectCoordinate(*managerObjects->camera->positionX));
  bufCamera->set_allocated_positiony(this->getObjectCoordinate(*managerObjects->camera->positionY));
  bufCamera->set_allocated_positionz(this->getObjectCoordinate(*managerObjects->camera->positionZ));
  bufCamera->set_allocated_rotatex(this->getObjectCoordinate(*managerObjects->camera->rotateX));
  bufCamera->set_allocated_rotatey(this->getObjectCoordinate(*managerObjects->camera->rotateY));
  bufCamera->set_allocated_rotatez(this->getObjectCoordinate(*managerObjects->camera->rotateZ));
  bufCamera->set_allocated_rotatecenterx(this->getObjectCoordinate(*managerObjects->camera->rotateCenterX));
  bufCamera->set_allocated_rotatecentery(this->getObjectCoordinate(*managerObjects->camera->rotateCenterY));
  bufCamera->set_allocated_rotatecenterz(this->getObjectCoordinate(*managerObjects->camera->rotateCenterZ));
  this->bufGUISettings.set_allocated_camera(bufCamera);

  KuplungApp::GridSettings* bufGrid = new KuplungApp::GridSettings();
  bufGrid->set_actasmirror(managerObjects->grid->actAsMirror);
  bufGrid->set_gridsize(managerObjects->grid->gridSize);
  bufGrid->set_allocated_positionx(this->getObjectCoordinate(*managerObjects->grid->positionX));
  bufGrid->set_allocated_positiony(this->getObjectCoordinate(*managerObjects->grid->positionY));
  bufGrid->set_allocated_positionz(this->getObjectCoordinate(*managerObjects->grid->positionZ));
  bufGrid->set_allocated_rotatex(this->getObjectCoordinate(*managerObjects->grid->rotateX));
  bufGrid->set_allocated_rotatey(this->getObjectCoordinate(*managerObjects->grid->rotateY));
  bufGrid->set_allocated_rotatez(this->getObjectCoordinate(*managerObjects->grid->rotateZ));
  bufGrid->set_allocated_scalex(this->getObjectCoordinate(*managerObjects->grid->scaleX));
  bufGrid->set_allocated_scaley(this->getObjectCoordinate(*managerObjects->grid->scaleY));
  bufGrid->set_allocated_scalez(this->getObjectCoordinate(*managerObjects->grid->scaleZ));
  bufGrid->set_transparency(managerObjects->grid->transparency);
  bufGrid->set_showgrid(managerObjects->grid->showGrid);
  this->bufGUISettings.set_allocated_grid(bufGrid);
}

void SaveOpenGProtocolBufs::readObjectsManagerSettings(std::unique_ptr<ObjectsManager>& managerObjects) const {
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

void SaveOpenGProtocolBufs::storeGlobalLights(std::unique_ptr<ObjectsManager>& managerObjects) {
  this->bufGUISettings.clear_lights();
  for (size_t i = 0; i < managerObjects->lightSources.size(); i++) {
    Light* l = managerObjects->lightSources[i];
    KuplungApp::LightObject* lo = this->bufGUISettings.add_lights();
    lo->set_title(l->title);
    lo->set_description(l->description);
    lo->set_type(l->type);
    lo->set_showlampobject(l->showLampObject);
    lo->set_showlampdirection(l->showLampDirection);
    lo->set_showinwire(l->showInWire);
    lo->set_allocated_positionx(this->getObjectCoordinate(*l->positionX));
    lo->set_allocated_positiony(this->getObjectCoordinate(*l->positionY));
    lo->set_allocated_positionz(this->getObjectCoordinate(*l->positionZ));
    lo->set_allocated_directionx(this->getObjectCoordinate(*l->directionX));
    lo->set_allocated_directiony(this->getObjectCoordinate(*l->directionY));
    lo->set_allocated_directionz(this->getObjectCoordinate(*l->directionZ));
    lo->set_allocated_scalex(this->getObjectCoordinate(*l->scaleX));
    lo->set_allocated_scaley(this->getObjectCoordinate(*l->scaleY));
    lo->set_allocated_scalez(this->getObjectCoordinate(*l->scaleZ));
    lo->set_allocated_rotatex(this->getObjectCoordinate(*l->rotateX));
    lo->set_allocated_rotatey(this->getObjectCoordinate(*l->rotateY));
    lo->set_allocated_rotatez(this->getObjectCoordinate(*l->rotateZ));
    lo->set_allocated_rotatecenterx(this->getObjectCoordinate(*l->rotateCenterX));
    lo->set_allocated_rotatecentery(this->getObjectCoordinate(*l->rotateCenterY));
    lo->set_allocated_rotatecenterz(this->getObjectCoordinate(*l->rotateCenterZ));
    lo->set_allocated_ambient(this->getMaterialColor(*l->ambient));
    lo->set_allocated_diffuse(this->getMaterialColor(*l->diffuse));
    lo->set_allocated_specular(this->getMaterialColor(*l->specular));
    lo->set_allocated_lcutoff(this->getObjectCoordinate(*l->lCutOff));
    lo->set_allocated_loutercutoff(this->getObjectCoordinate(*l->lOuterCutOff));
    lo->set_allocated_lconstant(this->getObjectCoordinate(*l->lConstant));
    lo->set_allocated_llinear(this->getObjectCoordinate(*l->lLinear));
    lo->set_allocated_lquadratic(this->getObjectCoordinate(*l->lQuadratic));
  }
}

void SaveOpenGProtocolBufs::readGlobalLights(std::unique_ptr<ObjectsManager>& managerObjects) const {
  for (int i = 0; i < this->bufGUISettings.lights_size(); i++) {
    KuplungApp::LightObject lo = this->bufGUISettings.lights(i);

    LightSourceType t = LightSourceType_Directional;
    switch (lo.type()) {
      case 0:
        t = LightSourceType_Directional;
        break;
      case 1:
        t = LightSourceType_Point;
        break;
      case 2:
        t = LightSourceType_Spot;
        break;
    }
    Light* l = new Light();
    l->title = lo.title();
    l->type = t;
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
    l->ambient = this->setMaterialColor(lo.ambient());
    l->diffuse = this->setMaterialColor(lo.diffuse());
    l->specular = this->setMaterialColor(lo.specular());
    l->lCutOff = this->setObjectCoordinate(lo.rotatecenterz());
    l->lOuterCutOff = this->setObjectCoordinate(lo.rotatecenterz());
    l->lConstant = this->setObjectCoordinate(lo.rotatecenterz());
    l->lLinear = this->setObjectCoordinate(lo.rotatecenterz());
    l->lQuadratic = this->setObjectCoordinate(lo.rotatecenterz());

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
  this->bufScene.clear_models();
  for (size_t i = 0; i < meshModelFaces.size(); i++) {
    ModelFaceBase* mf = meshModelFaces[i];
    KuplungApp::MeshModel* model = this->bufScene.add_models();
    model->set_modelid(mf->ModelID);
    model->set_settings_deferredrender(mf->Settings_DeferredRender);
    model->set_setting_celshading(mf->Setting_CelShading);
    model->set_setting_wireframe(mf->Setting_Wireframe);
    model->set_setting_usetessellation(mf->Setting_UseTessellation);
    model->set_setting_usecullface(mf->Setting_UseCullFace);
    model->set_setting_alpha(mf->Setting_Alpha);
    model->set_setting_tessellationsubdivision(mf->Setting_TessellationSubdivision);
    model->set_allocated_positionx(this->getObjectCoordinate(*mf->positionX));
    model->set_allocated_positiony(this->getObjectCoordinate(*mf->positionY));
    model->set_allocated_positionz(this->getObjectCoordinate(*mf->positionZ));
    model->set_allocated_scalex(this->getObjectCoordinate(*mf->scaleX));
    model->set_allocated_scaley(this->getObjectCoordinate(*mf->scaleY));
    model->set_allocated_scalez(this->getObjectCoordinate(*mf->scaleZ));
    model->set_allocated_rotatex(this->getObjectCoordinate(*mf->rotateX));
    model->set_allocated_rotatey(this->getObjectCoordinate(*mf->rotateY));
    model->set_allocated_rotatez(this->getObjectCoordinate(*mf->rotateZ));
    model->set_allocated_displacex(this->getObjectCoordinate(*mf->displaceX));
    model->set_allocated_displacey(this->getObjectCoordinate(*mf->displaceY));
    model->set_allocated_displacez(this->getObjectCoordinate(*mf->displaceZ));
    model->set_allocated_setting_materialrefraction(this->getObjectCoordinate(*mf->Setting_MaterialRefraction));
    model->set_allocated_setting_materialspecularexp(this->getObjectCoordinate(*mf->Setting_MaterialSpecularExp));
    model->set_setting_modelviewskin(mf->Setting_ModelViewSkin);
    model->set_allocated_solidlightskin_materialcolor(this->getVec3(mf->solidLightSkin_MaterialColor));
    model->set_allocated_solidlightskin_ambient(this->getVec3(mf->solidLightSkin_Ambient));
    model->set_allocated_solidlightskin_diffuse(this->getVec3(mf->solidLightSkin_Diffuse));
    model->set_allocated_solidlightskin_specular(this->getVec3(mf->solidLightSkin_Specular));
    model->set_solidlightskin_ambient_strength(mf->solidLightSkin_Ambient_Strength);
    model->set_solidlightskin_diffuse_strength(mf->solidLightSkin_Diffuse_Strength);
    model->set_solidlightskin_specular_strength(mf->solidLightSkin_Specular_Strength);
    model->set_allocated_setting_lightposition(this->getVec3(mf->Setting_LightPosition));
    model->set_allocated_setting_lightdirection(this->getVec3(mf->Setting_LightDirection));
    model->set_allocated_setting_lightambient(this->getVec3(mf->Setting_LightAmbient));
    model->set_allocated_setting_lightdiffuse(this->getVec3(mf->Setting_LightDiffuse));
    model->set_allocated_setting_lightspecular(this->getVec3(mf->Setting_LightSpecular));
    model->set_setting_lightstrengthambient(mf->Setting_LightStrengthAmbient);
    model->set_setting_lightstrengthdiffuse(mf->Setting_LightStrengthDiffuse);
    model->set_setting_lightstrengthspecular(mf->Setting_LightStrengthSpecular);
    model->set_materialilluminationmodel(mf->materialIlluminationModel);
    model->set_allocated_displacementheightscale(this->getObjectCoordinate(*mf->displacementHeightScale));
    model->set_showmaterialeditor(mf->showMaterialEditor);
    model->set_allocated_materialambient(this->getMaterialColor(*mf->materialAmbient));
    model->set_allocated_materialdiffuse(this->getMaterialColor(*mf->materialDiffuse));
    model->set_allocated_materialspecular(this->getMaterialColor(*mf->materialSpecular));
    model->set_allocated_materialemission(this->getMaterialColor(*mf->materialEmission));
    model->set_setting_parallaxmapping(mf->Setting_ParallaxMapping);
    model->set_effect_gblur_mode(mf->Effect_GBlur_Mode);
    model->set_allocated_effect_gblur_radius(this->getObjectCoordinate(*mf->Effect_GBlur_Radius));
    model->set_allocated_effect_gblur_width(this->getObjectCoordinate(*mf->Effect_GBlur_Width));
    model->set_effect_bloom_dobloom(mf->Effect_Bloom_doBloom);
    model->set_effect_bloom_weighta(mf->Effect_Bloom_WeightA);
    model->set_effect_bloom_weightb(mf->Effect_Bloom_WeightB);
    model->set_effect_bloom_weightc(mf->Effect_Bloom_WeightC);
    model->set_effect_bloom_weightd(mf->Effect_Bloom_WeightD);
    model->set_effect_bloom_vignette(mf->Effect_Bloom_Vignette);
    model->set_effect_bloom_vignetteatt(mf->Effect_Bloom_VignetteAtt);
    model->set_setting_lightingpass_drawmode(mf->Setting_LightingPass_DrawMode);
    model->set_allocated_meshobject(this->getMesh(mf->meshModel));
  }
}

const std::vector<ModelFaceData*> SaveOpenGProtocolBufs::readObjects(std::unique_ptr<ObjectsManager>& managerObjects) const {
  std::vector<ModelFaceData*> models;

  for (int i = 0; i < this->bufScene.models_size(); i++) {
    const KuplungApp::MeshModel& mm = this->bufScene.models(i);

    ModelFaceData* m = new ModelFaceData();
    m->initModelProperties();
    m->lightSources = managerObjects->lightSources;
    m->dataVertices = managerObjects->grid->dataVertices;
    m->dataTexCoords = managerObjects->grid->dataTexCoords;
    m->dataNormals = managerObjects->grid->dataNormals;
    m->dataIndices = managerObjects->grid->dataIndices;

    m->meshModel = this->setMesh(mm.meshobject());
    m->init(m->meshModel, Settings::Instance()->currentFolder);

    m->ModelID = mm.modelid();
    m->Settings_DeferredRender = mm.settings_deferredrender();
    m->Setting_CelShading = mm.setting_celshading();
    m->Setting_Wireframe = mm.setting_wireframe();
    m->Setting_UseTessellation = mm.setting_usetessellation();
    m->Setting_UseCullFace = mm.setting_usecullface();
    m->Setting_Alpha = mm.setting_alpha();
    m->Setting_TessellationSubdivision = mm.setting_tessellationsubdivision();
    m->positionX = this->setObjectCoordinate(mm.positionx());
    m->positionY = this->setObjectCoordinate(mm.positiony());
    m->positionZ = this->setObjectCoordinate(mm.positionz());
    m->scaleX = this->setObjectCoordinate(mm.scalex());
    m->scaleY = this->setObjectCoordinate(mm.scaley());
    m->scaleZ = this->setObjectCoordinate(mm.scalez());
    m->rotateX = this->setObjectCoordinate(mm.rotatex());
    m->rotateY = this->setObjectCoordinate(mm.rotatey());
    m->rotateZ = this->setObjectCoordinate(mm.rotatez());
    m->displaceX = this->setObjectCoordinate(mm.displacex());
    m->displaceY = this->setObjectCoordinate(mm.displacey());
    m->displaceZ = this->setObjectCoordinate(mm.displacez());
    m->Setting_MaterialRefraction = this->setObjectCoordinate(mm.setting_materialrefraction());
    m->Setting_MaterialSpecularExp = this->setObjectCoordinate(mm.setting_materialspecularexp());
    switch (mm.setting_modelviewskin()) {
      case 0:
        m->Setting_ModelViewSkin = ViewModelSkin_Solid;
        break;
      case 1:
        m->Setting_ModelViewSkin = ViewModelSkin_Material;
        break;
      case 2:
        m->Setting_ModelViewSkin = ViewModelSkin_Texture;
        break;
      case 3:
        m->Setting_ModelViewSkin = ViewModelSkin_Wireframe;
        break;
      case 4:
        m->Setting_ModelViewSkin = ViewModelSkin_Rendered;
        break;
    }
    m->solidLightSkin_MaterialColor = this->setVec3(mm.solidlightskin_materialcolor());
    m->solidLightSkin_Ambient = this->setVec3(mm.solidlightskin_ambient());
    m->solidLightSkin_Diffuse = this->setVec3(mm.solidlightskin_diffuse());
    m->solidLightSkin_Specular = this->setVec3(mm.solidlightskin_specular());
    m->solidLightSkin_Ambient_Strength = mm.solidlightskin_ambient_strength();
    m->solidLightSkin_Diffuse_Strength = mm.solidlightskin_diffuse_strength();
    m->solidLightSkin_Specular_Strength = mm.solidlightskin_specular_strength();
    m->Setting_LightPosition = this->setVec3(mm.setting_lightposition());
    m->Setting_LightDirection = this->setVec3(mm.setting_lightdirection());
    m->Setting_LightAmbient = this->setVec3(mm.setting_lightambient());
    m->Setting_LightDiffuse = this->setVec3(mm.setting_lightdiffuse());
    m->Setting_LightSpecular = this->setVec3(mm.setting_lightspecular());
    m->Setting_LightStrengthAmbient = mm.setting_lightstrengthambient();
    m->Setting_LightStrengthDiffuse = mm.setting_lightstrengthdiffuse();
    m->Setting_LightStrengthSpecular = mm.setting_lightstrengthspecular();
    m->materialIlluminationModel = mm.materialilluminationmodel();
    m->displacementHeightScale = this->setObjectCoordinate(mm.displacementheightscale());
    m->showMaterialEditor = mm.showmaterialeditor();
    m->materialAmbient = this->setMaterialColor(mm.materialambient());
    m->materialDiffuse = this->setMaterialColor(mm.materialdiffuse());
    m->materialSpecular = this->setMaterialColor(mm.materialspecular());
    m->materialEmission = this->setMaterialColor(mm.materialemission());
    m->Setting_ParallaxMapping = mm.setting_parallaxmapping();
    m->Effect_GBlur_Mode = mm.effect_gblur_mode();
    m->Effect_GBlur_Radius = this->setObjectCoordinate(mm.effect_gblur_radius());
    m->Effect_GBlur_Width = this->setObjectCoordinate(mm.effect_gblur_width());
    m->Effect_Bloom_doBloom = mm.effect_bloom_dobloom();
    m->Effect_Bloom_WeightA = mm.effect_bloom_weighta();
    m->Effect_Bloom_WeightB = mm.effect_bloom_weightb();
    m->Effect_Bloom_WeightC = mm.effect_bloom_weightc();
    m->Effect_Bloom_WeightD = mm.effect_bloom_weightd();
    m->Effect_Bloom_Vignette = mm.effect_bloom_vignette();
    m->Effect_Bloom_VignetteAtt = mm.effect_bloom_vignetteatt();
    m->Setting_LightingPass_DrawMode = mm.setting_lightingpass_drawmode();

    m->initBoundingBox();

    models.push_back(m);
  }

  return models;
}

KuplungApp::Mesh* SaveOpenGProtocolBufs::getMesh(MeshModel ent) const {
  KuplungApp::Mesh* mesh = new KuplungApp::Mesh();
  mesh->set_id(ent.ID);
  mesh->set_allocated_file(this->getFBEntity(ent.File));
  mesh->set_modeltitle(ent.ModelTitle);
  mesh->set_materialtitle(ent.MaterialTitle);
  mesh->set_countvertices(ent.countVertices);
  mesh->set_countnormals(ent.countNormals);
  mesh->set_counttexturecoordinates(ent.countTextureCoordinates);
  mesh->set_countindices(ent.countIndices);
  mesh->set_allocated_modelmaterial(this->getMeshModelMaterial(ent.ModelMaterial));
  for (size_t i = 0; i < ent.vertices.size(); i++) {
    glm::vec3 mv = ent.vertices[i];
    KuplungApp::Vec3* v = mesh->add_vertices();
    v->set_x(mv.x);
    v->set_y(mv.y);
    v->set_z(mv.z);
  }
  for (size_t i = 0; i < ent.normals.size(); i++) {
    glm::vec3 mv = ent.normals[i];
    KuplungApp::Vec3* v = mesh->add_normals();
    v->set_x(mv.x);
    v->set_y(mv.y);
    v->set_z(mv.z);
  }
  for (size_t i = 0; i < ent.texture_coordinates.size(); i++) {
    glm::vec2 mv = ent.texture_coordinates[i];
    KuplungApp::Vec2* v = mesh->add_texture_coordinates();
    v->set_x(mv.x);
    v->set_y(mv.y);
  }
  for (size_t i = 0; i < ent.indices.size(); i++) {
    mesh->add_indices(ent.indices[i]);
  }
  return mesh;
}

MeshModel SaveOpenGProtocolBufs::setMesh(const KuplungApp::Mesh& ent) const {
  MeshModel mm;
  mm.ID = ent.id();
  mm.File = this->setFBEntity(ent.file());
  mm.ModelTitle = ent.modeltitle();
  mm.MaterialTitle = ent.materialtitle();
  mm.countVertices = ent.countvertices();
  mm.countNormals = ent.countnormals();
  mm.countTextureCoordinates = ent.counttexturecoordinates();
  mm.countIndices = ent.countindices();
  mm.ModelMaterial = this->setMeshModelMaterial(ent.modelmaterial());
  for (int i = 0; i < ent.vertices_size(); i++) {
    const KuplungApp::Vec3& v = ent.vertices(i);
    mm.vertices.push_back(glm::vec3(v.x(), v.y(), v.z()));
  }
  for (int i = 0; i < ent.normals_size(); i++) {
    const KuplungApp::Vec3& v = ent.normals(i);
    mm.normals.push_back(glm::vec3(v.x(), v.y(), v.z()));
  }
  for (int i = 0; i < ent.texture_coordinates_size(); i++) {
    const KuplungApp::Vec2& v = ent.texture_coordinates(i);
    mm.texture_coordinates.push_back(glm::vec2(v.x(), v.y()));
  }
  for (int i = 0; i < ent.indices_size(); i++) {
    mm.indices.push_back(ent.indices(i));
  }
  return mm;
}

KuplungApp::MeshModelMaterial* SaveOpenGProtocolBufs::getMeshModelMaterial(MeshModelMaterial ent) const {
  KuplungApp::MeshModelMaterial* m = new KuplungApp::MeshModelMaterial();
  m->set_materialid(ent.MaterialID);
  m->set_materialtitle(ent.MaterialTitle);
  m->set_allocated_ambientcolor(this->getVec3(ent.AmbientColor));
  m->set_allocated_diffusecolor(this->getVec3(ent.DiffuseColor));
  m->set_allocated_specularcolor(this->getVec3(ent.SpecularColor));
  m->set_allocated_emissioncolor(this->getVec3(ent.EmissionColor));
  m->set_specularexp(ent.SpecularExp);
  m->set_transparency(ent.Transparency);
  m->set_illuminationmode(ent.IlluminationMode);
  m->set_opticaldensity(ent.OpticalDensity);
  m->set_allocated_textureambient(this->getMeshMaterialTextureImage(ent.TextureAmbient));
  m->set_allocated_texturediffuse(this->getMeshMaterialTextureImage(ent.TextureDiffuse));
  m->set_allocated_texturespecular(this->getMeshMaterialTextureImage(ent.TextureSpecular));
  m->set_allocated_texturespecularexp(this->getMeshMaterialTextureImage(ent.TextureSpecularExp));
  m->set_allocated_texturedissolve(this->getMeshMaterialTextureImage(ent.TextureDissolve));
  m->set_allocated_texturebump(this->getMeshMaterialTextureImage(ent.TextureBump));
  m->set_allocated_texturedisplacement(this->getMeshMaterialTextureImage(ent.TextureDisplacement));
  return m;
}

MeshModelMaterial SaveOpenGProtocolBufs::setMeshModelMaterial(const KuplungApp::MeshModelMaterial& ent) const {
  MeshModelMaterial m;
  m.MaterialID = ent.materialid();
  m.MaterialTitle = ent.materialtitle();
  m.AmbientColor = this->setVec3(ent.ambientcolor());
  m.DiffuseColor = this->setVec3(ent.diffusecolor());
  m.SpecularColor = this->setVec3(ent.specularcolor());
  m.EmissionColor = this->setVec3(ent.emissioncolor());
  m.SpecularExp = ent.specularexp();
  m.Transparency = ent.transparency();
  m.IlluminationMode = ent.illuminationmode();
  m.OpticalDensity = ent.opticaldensity();
  m.TextureAmbient = this->setMeshMaterialTextureImage(ent.textureambient());
  m.TextureDiffuse = this->setMeshMaterialTextureImage(ent.texturediffuse());
  m.TextureSpecular = this->setMeshMaterialTextureImage(ent.texturespecular());
  m.TextureSpecularExp = this->setMeshMaterialTextureImage(ent.texturespecularexp());
  m.TextureDissolve = this->setMeshMaterialTextureImage(ent.texturedissolve());
  m.TextureBump = this->setMeshMaterialTextureImage(ent.texturebump());
  m.TextureDisplacement = this->setMeshMaterialTextureImage(ent.texturedisplacement());
  return m;
}

KuplungApp::FBEntity* SaveOpenGProtocolBufs::getFBEntity(FBEntity ent) const {
  KuplungApp::FBEntity* fb = new KuplungApp::FBEntity();
  fb->set_extension(ent.extension);
  fb->set_isfile(ent.isFile);
  fb->set_modifieddate(ent.modifiedDate);
  fb->set_path(ent.path);
  fb->set_size(ent.size);
  fb->set_title(ent.title);
  return fb;
}

FBEntity SaveOpenGProtocolBufs::setFBEntity(const KuplungApp::FBEntity& ent) const {
  FBEntity fb;
  fb.extension = ent.extension();
  fb.isFile = ent.isfile();
  fb.modifiedDate = ent.modifieddate();
  fb.path = ent.path();
  fb.size = ent.size();
  fb.title = ent.title();
  return fb;
}

KuplungApp::MeshMaterialTextureImage* SaveOpenGProtocolBufs::getMeshMaterialTextureImage(MeshMaterialTextureImage ent) const {
  KuplungApp::MeshMaterialTextureImage* meshMatTexImage = new KuplungApp::MeshMaterialTextureImage();
  meshMatTexImage->set_filename(ent.Filename);
  meshMatTexImage->set_image(ent.Image);
  meshMatTexImage->set_width(ent.Width);
  meshMatTexImage->set_height(ent.Height);
  meshMatTexImage->set_usetexture(ent.UseTexture);
  for (size_t i = 0; i < ent.Commands.size(); i++) {
    std::string* s = meshMatTexImage->add_commands();
    *s = ent.Commands[i];
  }
  return meshMatTexImage;
}

const MeshMaterialTextureImage SaveOpenGProtocolBufs::setMeshMaterialTextureImage(const KuplungApp::MeshMaterialTextureImage& ent) const {
  MeshMaterialTextureImage meshMatTexImage;
  meshMatTexImage.Filename = ent.filename();
  meshMatTexImage.Image = ent.image();
  meshMatTexImage.Width = ent.width();
  meshMatTexImage.Height = ent.height();
  meshMatTexImage.UseTexture = ent.usetexture();
  for (int i = 0; i < ent.commands_size(); i++) {
    meshMatTexImage.Commands.push_back(ent.commands(i));
  }
  return meshMatTexImage;
}

KuplungApp::MaterialColor* SaveOpenGProtocolBufs::getMaterialColor(MaterialColor& v) const {
  KuplungApp::MaterialColor* mc = new KuplungApp::MaterialColor();
  mc->set_colorpickeropen(v.colorPickerOpen);
  mc->set_animate(v.animate);
  mc->set_strength(v.strength);
  mc->set_allocated_color(this->getVec3(v.color));
  return mc;
}

std::unique_ptr<MaterialColor> SaveOpenGProtocolBufs::setMaterialColor(const KuplungApp::MaterialColor& v) const {
  auto t = std::make_unique<MaterialColor>(v.colorpickeropen(), v.animate(), v.strength(), this->setVec3(v.color()));
  return t;
}

KuplungApp::Vec4* SaveOpenGProtocolBufs::getVec4(const glm::vec4& v) const {
  KuplungApp::Vec4* gv = new KuplungApp::Vec4();
  gv->set_x(v.x);
  gv->set_y(v.y);
  gv->set_z(v.z);
  gv->set_w(v.w);
  return gv;
}

KuplungApp::Vec3* SaveOpenGProtocolBufs::getVec3(const glm::vec3& v) const {
  KuplungApp::Vec3* gv = new KuplungApp::Vec3();
  gv->set_x(v.x);
  gv->set_y(v.y);
  gv->set_z(v.z);
  return gv;
}

glm::vec4 SaveOpenGProtocolBufs::setVec4(const KuplungApp::Vec4& v) const {
  return glm::vec4(v.x(), v.y(), v.z(), v.w());
}

glm::vec3 SaveOpenGProtocolBufs::setVec3(const KuplungApp::Vec3& v) const {
  return glm::vec3(v.x(), v.y(), v.z());
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

KuplungApp::ObjectCoordinate* SaveOpenGProtocolBufs::getObjectCoordinate(ObjectCoordinate& v) const {
  KuplungApp::ObjectCoordinate* oc = new KuplungApp::ObjectCoordinate();
  oc->set_animate(v.animate);
  oc->set_point(v.point);
  return oc;
}

std::unique_ptr<ObjectCoordinate> SaveOpenGProtocolBufs::setObjectCoordinate(const KuplungApp::ObjectCoordinate& v) const {
  auto t = std::make_unique<ObjectCoordinate>(v.animate(), v.point());
  return t;
}

const bool SaveOpenGProtocolBufs::hasEnding(std::string const& fullString, std::string const& ending) const {
  if (fullString.length() >= ending.length())
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  else
    return false;
}
