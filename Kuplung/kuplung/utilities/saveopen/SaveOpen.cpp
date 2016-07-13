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

    this->binary_write(kuplungFile, managerObjects->Setting_FOV);
    this->binary_write(kuplungFile, managerObjects->Setting_OutlineThickness);
    this->binary_write(kuplungFile, managerObjects->Setting_RatioWidth);
    this->binary_write(kuplungFile, managerObjects->Setting_RatioHeight);
    this->binary_write(kuplungFile, managerObjects->Setting_PlaneClose);
    this->binary_write(kuplungFile, managerObjects->Setting_PlaneFar);
    this->binary_write(kuplungFile, managerObjects->Setting_GridSize);
    this->binary_write(kuplungFile, managerObjects->Setting_Skybox);
    this->binary_write(kuplungFile, managerObjects->Setting_OutlineColor);
    this->binary_write(kuplungFile, managerObjects->Setting_UIAmbientLight);
    this->binary_write(kuplungFile, managerObjects->Setting_FixedGridWorld);
    this->binary_write(kuplungFile, managerObjects->Setting_OutlineColorPickerOpen);
    this->binary_write(kuplungFile, managerObjects->Setting_ShowAxisHelpers);
    this->binary_write(kuplungFile, managerObjects->Settings_ShowZAxis);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Direction);
    this->binary_write(kuplungFile, managerObjects->SolidLight_MaterialColor);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Ambient);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Diffuse);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Specular);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Ambient_Strength);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Diffuse_Strength);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Specular_Strength);
    this->binary_write(kuplungFile, managerObjects->SolidLight_MaterialColor_ColorPicker);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Ambient_ColorPicker);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Diffuse_ColorPicker);
    this->binary_write(kuplungFile, managerObjects->SolidLight_Specular_ColorPicker);
    this->binary_write(kuplungFile, managerObjects->Setting_ShowTerrain);
    this->binary_write(kuplungFile, managerObjects->Setting_TerrainModel);
    this->binary_write(kuplungFile, managerObjects->Setting_TerrainAnimateX);
    this->binary_write(kuplungFile, managerObjects->Setting_TerrainAnimateY);
    this->binary_write(kuplungFile, managerObjects->Setting_TerrainWidth);
    this->binary_write(kuplungFile, managerObjects->Setting_TerrainHeight);
    this->binary_write(kuplungFile, managerObjects->Setting_DeferredTestMode);
    this->binary_write(kuplungFile, managerObjects->Setting_DeferredTestLights);
    this->binary_write(kuplungFile, managerObjects->Setting_LightingPass_DrawMode);
    this->binary_write(kuplungFile, managerObjects->Setting_DeferredAmbientStrength);
    this->binary_write(kuplungFile, managerObjects->Setting_DeferredTestLightsNumber);
    this->binary_write(kuplungFile, managerObjects->Setting_ShowSpaceship);
    this->binary_write(kuplungFile, managerObjects->Setting_GenerateSpaceship);

    kuplungFile.close();
}

void SaveOpen::openKuplungFile(FBEntity file, ObjectsManager *managerObjects) {
    std::string fileName = file.path + "/" + file.title;

    std::ifstream kuplungFile;
    kuplungFile.open(fileName.c_str(), std::ios::binary | std::ios::out | std::ios::app);

    if (kuplungFile.is_open() && !kuplungFile.bad()) {
        kuplungFile.seekg(0);

        this->binary_read(kuplungFile, managerObjects->Setting_FOV);
        this->binary_read(kuplungFile, managerObjects->Setting_OutlineThickness);
        this->binary_read(kuplungFile, managerObjects->Setting_RatioWidth);
        this->binary_read(kuplungFile, managerObjects->Setting_RatioHeight);
        this->binary_read(kuplungFile, managerObjects->Setting_PlaneClose);
        this->binary_read(kuplungFile, managerObjects->Setting_PlaneFar);
        this->binary_read(kuplungFile, managerObjects->Setting_GridSize);
        this->binary_read(kuplungFile, managerObjects->Setting_Skybox);
        this->binary_read(kuplungFile, managerObjects->Setting_OutlineColor);
        this->binary_read(kuplungFile, managerObjects->Setting_UIAmbientLight);
        this->binary_read(kuplungFile, managerObjects->Setting_FixedGridWorld);
        this->binary_read(kuplungFile, managerObjects->Setting_OutlineColorPickerOpen);
        this->binary_read(kuplungFile, managerObjects->Setting_ShowAxisHelpers);
        this->binary_read(kuplungFile, managerObjects->Settings_ShowZAxis);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Direction);
        this->binary_read(kuplungFile, managerObjects->SolidLight_MaterialColor);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Ambient);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Diffuse);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Specular);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Ambient_Strength);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Diffuse_Strength);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Specular_Strength);
        this->binary_read(kuplungFile, managerObjects->SolidLight_MaterialColor_ColorPicker);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Ambient_ColorPicker);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Diffuse_ColorPicker);
        this->binary_read(kuplungFile, managerObjects->SolidLight_Specular_ColorPicker);
        this->binary_read(kuplungFile, managerObjects->Setting_ShowTerrain);
        this->binary_read(kuplungFile, managerObjects->Setting_TerrainModel);
        this->binary_read(kuplungFile, managerObjects->Setting_TerrainAnimateX);
        this->binary_read(kuplungFile, managerObjects->Setting_TerrainAnimateY);
        this->binary_read(kuplungFile, managerObjects->Setting_TerrainWidth);
        this->binary_read(kuplungFile, managerObjects->Setting_TerrainHeight);
        this->binary_read(kuplungFile, managerObjects->Setting_DeferredTestMode);
        this->binary_read(kuplungFile, managerObjects->Setting_DeferredTestLights);
        this->binary_read(kuplungFile, managerObjects->Setting_LightingPass_DrawMode);
        this->binary_read(kuplungFile, managerObjects->Setting_DeferredAmbientStrength);
        this->binary_read(kuplungFile, managerObjects->Setting_DeferredTestLightsNumber);
        this->binary_read(kuplungFile, managerObjects->Setting_ShowSpaceship);
        this->binary_read(kuplungFile, managerObjects->Setting_GenerateSpaceship);

        kuplungFile.close();
    }
}

bool SaveOpen::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}

template<typename T>
std::ostream& SaveOpen::binary_write(std::ostream& stream, const T& value) {
    return stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
std::istream& SaveOpen::binary_read(std::istream& stream, T& value) {
    return stream.read(reinterpret_cast<char*>(&value), sizeof(T));
}
