//
//  SaveOpen.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SaveOpen.hpp"
#include <iostream>
#include <fstream>

void SaveOpen::init() {
}

void SaveOpen::saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces) {
    std::string fileName = file.path + "/" + file.title;
    if (!this->hasEnding(fileName, ".kuplung"))
        fileName += ".kuplung";

    std::remove(fileName.c_str());

    std::ofstream kuplungFile;
    kuplungFile.open(fileName.c_str(), std::ios::binary | std::ios::out | std::ios::app);

    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_FOV), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_OutlineThickness), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_RatioWidth), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_RatioHeight), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_PlaneClose), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_PlaneFar), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_GridSize), sizeof(int));
    kuplungFile.write(reinterpret_cast<const char*>(&managerObjects->Setting_Skybox), sizeof(int));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_OutlineColor), sizeof(glm::vec4));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_UIAmbientLight), sizeof(glm::vec3));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_FixedGridWorld), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_OutlineColorPickerOpen), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_ShowAxisHelpers), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Settings_ShowZAxis), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Direction), sizeof(glm::vec3));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_MaterialColor), sizeof(glm::vec3));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Ambient), sizeof(glm::vec3));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Diffuse), sizeof(glm::vec3));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Specular), sizeof(glm::vec3));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Ambient_Strength), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Diffuse_Strength), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Specular_Strength), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_MaterialColor_ColorPicker), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Ambient_ColorPicker), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Diffuse_ColorPicker), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->SolidLight_Specular_ColorPicker), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_ShowTerrain), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_TerrainModel), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_TerrainAnimateX), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_TerrainAnimateY), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_TerrainWidth), sizeof(int));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_TerrainHeight), sizeof(int));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_DeferredTestMode), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_DeferredTestLights), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_LightingPass_DrawMode), sizeof(int));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_DeferredAmbientStrength), sizeof(float));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_DeferredTestLightsNumber), sizeof(int));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_ShowSpaceship), sizeof(bool));
    kuplungFile.write(reinterpret_cast<const char *>(&managerObjects->Setting_GenerateSpaceship), sizeof(bool));

    kuplungFile.close();
}

void SaveOpen::openKuplungFile(FBEntity file, ObjectsManager *managerObjects) {
    std::string fileName = file.path + "/" + file.title;

    std::ifstream kuplungFile;
    kuplungFile.open(fileName.c_str(), std::ios::binary | std::ios::out | std::ios::app);

    if (kuplungFile.is_open() && !kuplungFile.bad()) {
        kuplungFile.seekg(0);

        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_FOV), sizeof(float));
        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_OutlineThickness), sizeof(float));
        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_RatioWidth), sizeof(float));
        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_RatioHeight), sizeof(float));
        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_PlaneClose), sizeof(float));
        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_PlaneFar), sizeof(float));
        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_GridSize), sizeof(int));
        kuplungFile.read(reinterpret_cast<char*>(&managerObjects->Setting_Skybox), sizeof(int));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_OutlineColor), sizeof(glm::vec4));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_UIAmbientLight), sizeof(glm::vec3));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_FixedGridWorld), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_OutlineColorPickerOpen), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_ShowAxisHelpers), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Settings_ShowZAxis), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Direction), sizeof(glm::vec3));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_MaterialColor), sizeof(glm::vec3));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Ambient), sizeof(glm::vec3));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Diffuse), sizeof(glm::vec3));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Specular), sizeof(glm::vec3));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Ambient_Strength), sizeof(float));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Diffuse_Strength), sizeof(float));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Specular_Strength), sizeof(float));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_MaterialColor_ColorPicker), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Ambient_ColorPicker), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Diffuse_ColorPicker), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->SolidLight_Specular_ColorPicker), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_ShowTerrain), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_TerrainModel), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_TerrainAnimateX), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_TerrainAnimateY), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_TerrainWidth), sizeof(int));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_TerrainHeight), sizeof(int));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_DeferredTestMode), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_DeferredTestLights), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_LightingPass_DrawMode), sizeof(int));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_DeferredAmbientStrength), sizeof(float));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_DeferredTestLightsNumber), sizeof(int));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_ShowSpaceship), sizeof(bool));
        kuplungFile.read(reinterpret_cast<char *>(&managerObjects->Setting_GenerateSpaceship), sizeof(bool));

        kuplungFile.close();
    }
}

bool SaveOpen::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}
