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
    this->binary_write(kuplungFile, managerObjects->viewModelSkin);
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
    this->binary_write(kuplungFile, managerObjects->matrixProjection);
    this->binary_write(kuplungFile, managerObjects->camera->cameraPosition);
    this->binary_write(kuplungFile, managerObjects->camera->eyeSettings->View_Eye);
    this->binary_write(kuplungFile, managerObjects->camera->eyeSettings->View_Center);
    this->binary_write(kuplungFile, managerObjects->camera->eyeSettings->View_Up);
    this->binary_write(kuplungFile, managerObjects->camera->positionX);
    this->binary_write(kuplungFile, managerObjects->camera->positionY);
    this->binary_write(kuplungFile, managerObjects->camera->positionZ);
    this->binary_write(kuplungFile, managerObjects->camera->rotateX);
    this->binary_write(kuplungFile, managerObjects->camera->rotateY);
    this->binary_write(kuplungFile, managerObjects->camera->rotateZ);
    this->binary_write(kuplungFile, managerObjects->camera->rotateCenterX);
    this->binary_write(kuplungFile, managerObjects->camera->rotateCenterY);
    this->binary_write(kuplungFile, managerObjects->camera->rotateCenterZ);
    this->binary_write(kuplungFile, managerObjects->grid->actAsMirror);
    this->binary_write(kuplungFile, managerObjects->grid->gridSize);
    this->binary_write(kuplungFile, managerObjects->grid->positionX);
    this->binary_write(kuplungFile, managerObjects->grid->positionY);
    this->binary_write(kuplungFile, managerObjects->grid->positionZ);
    this->binary_write(kuplungFile, managerObjects->grid->rotateX);
    this->binary_write(kuplungFile, managerObjects->grid->rotateY);
    this->binary_write(kuplungFile, managerObjects->grid->rotateZ);
    this->binary_write(kuplungFile, managerObjects->grid->scaleX);
    this->binary_write(kuplungFile, managerObjects->grid->scaleY);
    this->binary_write(kuplungFile, managerObjects->grid->scaleZ);
    this->binary_write(kuplungFile, managerObjects->grid->transparency);
    this->binary_write(kuplungFile, managerObjects->grid->showGrid);

    int lightsCount = int(managerObjects->lightSources.size());
    this->binary_write(kuplungFile, lightsCount);
    for (size_t i=0; i<managerObjects->lightSources.size(); i++) {
        Light* l = managerObjects->lightSources[i];
        this->binary_write(kuplungFile, l->description.c_str());
        this->binary_write(kuplungFile, l->showInWire);
        this->binary_write(kuplungFile, l->showLampDirection);
        this->binary_write(kuplungFile, l->showLampObject);
        this->binary_write(kuplungFile, l->title.c_str());
        this->binary_write(kuplungFile, l->type);
        this->binary_write(kuplungFile, l->ambient);
        this->binary_write(kuplungFile, l->diffuse);
        this->binary_write(kuplungFile, l->specular);
        this->binary_write(kuplungFile, l->positionX);
        this->binary_write(kuplungFile, l->positionY);
        this->binary_write(kuplungFile, l->positionZ);
        this->binary_write(kuplungFile, l->directionX);
        this->binary_write(kuplungFile, l->directionY);
        this->binary_write(kuplungFile, l->directionZ);
        this->binary_write(kuplungFile, l->scaleX);
        this->binary_write(kuplungFile, l->scaleY);
        this->binary_write(kuplungFile, l->scaleZ);
        this->binary_write(kuplungFile, l->rotateX);
        this->binary_write(kuplungFile, l->rotateY);
        this->binary_write(kuplungFile, l->rotateZ);
        this->binary_write(kuplungFile, l->rotateCenterX);
        this->binary_write(kuplungFile, l->rotateCenterY);
        this->binary_write(kuplungFile, l->rotateCenterZ);
        this->binary_write(kuplungFile, l->lCutOff);
        this->binary_write(kuplungFile, l->lOuterCutOff);
        this->binary_write(kuplungFile, l->lConstant);
        this->binary_write(kuplungFile, l->lLinear);
        this->binary_write(kuplungFile, l->lQuadratic);
        this->binary_write(kuplungFile, l->meshModel);
        this->binary_write_model(kuplungFile, l->meshModel);
    }

//    for (size_t i=0; i<meshModelFaces.size(); i++) {
//        this->binary_write(kuplungFile, meshModelFaces[i]);
//    }

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
        this->binary_read(kuplungFile, managerObjects->viewModelSkin);
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
        this->binary_read(kuplungFile, managerObjects->matrixProjection);
        this->binary_read(kuplungFile, managerObjects->camera->cameraPosition);
        this->binary_read(kuplungFile, managerObjects->camera->eyeSettings->View_Eye);
        this->binary_read(kuplungFile, managerObjects->camera->eyeSettings->View_Center);
        this->binary_read(kuplungFile, managerObjects->camera->eyeSettings->View_Up);
        this->binary_read(kuplungFile, managerObjects->camera->positionX);
        this->binary_read(kuplungFile, managerObjects->camera->positionY);
        this->binary_read(kuplungFile, managerObjects->camera->positionZ);
        this->binary_read(kuplungFile, managerObjects->camera->rotateX);
        this->binary_read(kuplungFile, managerObjects->camera->rotateY);
        this->binary_read(kuplungFile, managerObjects->camera->rotateZ);
        this->binary_read(kuplungFile, managerObjects->camera->rotateCenterX);
        this->binary_read(kuplungFile, managerObjects->camera->rotateCenterY);
        this->binary_read(kuplungFile, managerObjects->camera->rotateCenterZ);
        this->binary_read(kuplungFile, managerObjects->grid->actAsMirror);
        this->binary_read(kuplungFile, managerObjects->grid->gridSize);
        this->binary_read(kuplungFile, managerObjects->grid->positionX);
        this->binary_read(kuplungFile, managerObjects->grid->positionY);
        this->binary_read(kuplungFile, managerObjects->grid->positionZ);
        this->binary_read(kuplungFile, managerObjects->grid->rotateX);
        this->binary_read(kuplungFile, managerObjects->grid->rotateY);
        this->binary_read(kuplungFile, managerObjects->grid->rotateZ);
        this->binary_read(kuplungFile, managerObjects->grid->scaleX);
        this->binary_read(kuplungFile, managerObjects->grid->scaleY);
        this->binary_read(kuplungFile, managerObjects->grid->scaleZ);
        this->binary_read(kuplungFile, managerObjects->grid->transparency);
        this->binary_read(kuplungFile, managerObjects->grid->showGrid);

        managerObjects->lightSources.clear();
        int lightsCount = 0;
        this->binary_read(kuplungFile, lightsCount);
        for (size_t i=0; i<size_t(lightsCount); i++) {
            Light* l = new Light();
            l->init(LightSourceType_Directional);
            l->initProperties();

            char tempString = '\0';
            this->binary_read(kuplungFile, tempString);
            l->description = std::to_string(tempString);

            this->binary_read(kuplungFile, l->showInWire);
            this->binary_read(kuplungFile, l->showLampDirection);
            this->binary_read(kuplungFile, l->showLampObject);

            tempString = '\0';
            this->binary_read(kuplungFile, tempString);
            l->title = std::to_string(tempString);

            this->binary_read(kuplungFile, l->type);
            this->binary_read(kuplungFile, l->ambient);
            this->binary_read(kuplungFile, l->diffuse);
            this->binary_read(kuplungFile, l->specular);
            this->binary_read(kuplungFile, l->positionX);
            this->binary_read(kuplungFile, l->positionY);
            this->binary_read(kuplungFile, l->positionZ);
            this->binary_read(kuplungFile, l->directionX);
            this->binary_read(kuplungFile, l->directionY);
            this->binary_read(kuplungFile, l->directionZ);
            this->binary_read(kuplungFile, l->scaleX);
            this->binary_read(kuplungFile, l->scaleY);
            this->binary_read(kuplungFile, l->scaleZ);
            this->binary_read(kuplungFile, l->rotateX);
            this->binary_read(kuplungFile, l->rotateY);
            this->binary_read(kuplungFile, l->rotateZ);
            this->binary_read(kuplungFile, l->rotateCenterX);
            this->binary_read(kuplungFile, l->rotateCenterY);
            this->binary_read(kuplungFile, l->rotateCenterZ);
            this->binary_read(kuplungFile, l->lCutOff);
            this->binary_read(kuplungFile, l->lOuterCutOff);
            this->binary_read(kuplungFile, l->lConstant);
            this->binary_read(kuplungFile, l->lLinear);
            this->binary_read(kuplungFile, l->lQuadratic);
            l->meshModel = this->binary_read_model(kuplungFile);

            l->initBuffers(Settings::Instance()->appFolder());
            l->initShaderProgram();

            managerObjects->lightSources.push_back(l);
        }

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

void SaveOpen::binary_write_model(std::ostream& stream, MeshModel model) {
    this->binary_write(stream, model.File.extension.c_str());
    this->binary_write(stream, model.File.isFile);
    this->binary_write(stream, model.File.modifiedDate.c_str());
    this->binary_write(stream, model.File.path.c_str());
    this->binary_write(stream, model.File.size.c_str());
    this->binary_write(stream, model.File.title.c_str());
    this->binary_write(stream, model.ID);
    this->binary_write(stream, model.ModelTitle.c_str());
    this->binary_write(stream, model.MaterialTitle.c_str());
    this->binary_write(stream, model.countVertices);
    this->binary_write(stream, model.countTextureCoordinates);
    this->binary_write(stream, model.countNormals);
    this->binary_write(stream, model.countIndices);

    this->binary_write(stream, model.ModelMaterial.MaterialID);
    this->binary_write(stream, model.ModelMaterial.MaterialTitle.c_str());
    this->binary_write(stream, model.ModelMaterial.AmbientColor);
    this->binary_write(stream, model.ModelMaterial.DiffuseColor);
    this->binary_write(stream, model.ModelMaterial.SpecularColor);
    this->binary_write(stream, model.ModelMaterial.EmissionColor);
    this->binary_write(stream, model.ModelMaterial.SpecularExp);
    this->binary_write(stream, model.ModelMaterial.Transparency);
    this->binary_write(stream, model.ModelMaterial.IlluminationMode);
    this->binary_write(stream, model.ModelMaterial.OpticalDensity);
    this->binary_write_model_material_texture(stream, model.ModelMaterial.TextureAmbient);
    this->binary_write_model_material_texture(stream, model.ModelMaterial.TextureDiffuse);
    this->binary_write_model_material_texture(stream, model.ModelMaterial.TextureSpecular);
    this->binary_write_model_material_texture(stream, model.ModelMaterial.TextureSpecularExp);
    this->binary_write_model_material_texture(stream, model.ModelMaterial.TextureDissolve);
    this->binary_write_model_material_texture(stream, model.ModelMaterial.TextureBump);
    this->binary_write_model_material_texture(stream, model.ModelMaterial.TextureDisplacement);

    stream.write((char*)&model.vertices[0], model.vertices.size() * sizeof(glm::vec3));
    stream.write((char*)&model.texture_coordinates[0], model.texture_coordinates.size() * sizeof(glm::vec2));
    stream.write((char*)&model.normals[0], model.normals.size() * sizeof(glm::vec3));
    stream.write((char*)&model.indices[0], model.indices.size() * sizeof(unsigned int));
}

void SaveOpen::binary_write_model_material_texture(std::ostream& stream, MeshMaterialTextureImage materialTexture) {
    this->binary_write(stream, materialTexture.Filename.c_str());
    this->binary_write(stream, materialTexture.Image.c_str());
    this->binary_write(stream, materialTexture.Width);
    this->binary_write(stream, materialTexture.Height);
    this->binary_write(stream, materialTexture.UseTexture);
    this->binary_write(stream, materialTexture.UseTexture);

    int commandsCount = int(materialTexture.Commands.size());
    this->binary_write(stream, commandsCount);
    for (size_t i=0; i<size_t(commandsCount); i++) {
        this->binary_write(stream, materialTexture.Commands[i].c_str());
    }
}

MeshModel SaveOpen::binary_read_model(std::istream& stream) {
    MeshModel model;
    char tempString;

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.File.extension = std::to_string(tempString);

    this->binary_read(stream, model.File.isFile);

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.File.modifiedDate = std::to_string(tempString);

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.File.path = std::to_string(tempString);

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.File.size = std::to_string(tempString);

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.File.title = std::to_string(tempString);

    this->binary_read(stream, model.ID);

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.ModelTitle = std::to_string(tempString);

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.MaterialTitle = std::to_string(tempString);

    this->binary_read(stream, model.countVertices);
    this->binary_read(stream, model.countTextureCoordinates);
    this->binary_read(stream, model.countNormals);
    this->binary_read(stream, model.countIndices);
    this->binary_read(stream, model.ModelMaterial.MaterialID);

    tempString = '\0';
    this->binary_read(stream, tempString);
    model.ModelMaterial.MaterialTitle = std::to_string(tempString);

    this->binary_read(stream, model.ModelMaterial.AmbientColor);
    this->binary_read(stream, model.ModelMaterial.DiffuseColor);
    this->binary_read(stream, model.ModelMaterial.SpecularColor);
    this->binary_read(stream, model.ModelMaterial.EmissionColor);
    this->binary_read(stream, model.ModelMaterial.SpecularExp);
    this->binary_read(stream, model.ModelMaterial.Transparency);
    this->binary_read(stream, model.ModelMaterial.IlluminationMode);
    this->binary_read(stream, model.ModelMaterial.OpticalDensity);
    model.ModelMaterial.TextureAmbient = this->binary_read_model_material_texture(stream);
    model.ModelMaterial.TextureDiffuse = this->binary_read_model_material_texture(stream);
    model.ModelMaterial.TextureSpecular = this->binary_read_model_material_texture(stream);
    model.ModelMaterial.TextureSpecularExp = this->binary_read_model_material_texture(stream);
    model.ModelMaterial.TextureDissolve = this->binary_read_model_material_texture(stream);
    model.ModelMaterial.TextureBump = this->binary_read_model_material_texture(stream);
    model.ModelMaterial.TextureDisplacement = this->binary_read_model_material_texture(stream);

    stream.read((char*)&model.vertices[0], model.vertices.size() * sizeof(glm::vec3));
    stream.read((char*)&model.texture_coordinates[0], model.texture_coordinates.size() * sizeof(glm::vec2));
    stream.read((char*)&model.normals[0], model.normals.size() * sizeof(glm::vec3));
    stream.read((char*)&model.indices[0], model.indices.size() * sizeof(unsigned int));

    return model;
}

MeshMaterialTextureImage SaveOpen::binary_read_model_material_texture(std::istream& stream) {
    MeshMaterialTextureImage t;
    char tempString;

    tempString = '\0';
    this->binary_read(stream, tempString);
    t.Filename = std::to_string(tempString);

    tempString = '\0';
    this->binary_read(stream, tempString);
    t.Image = std::to_string(tempString);

    this->binary_read(stream, t.Width);
    this->binary_read(stream, t.Height);
    this->binary_read(stream, t.UseTexture);

    int lightsCount = 0;
    this->binary_read(stream, lightsCount);
    for (size_t i=0; i<t.Commands.size(); i++) {
        tempString = '\0';
        this->binary_read(stream, tempString);
        t.Commands[i] = std::to_string(tempString);
    }
    return t;
}
