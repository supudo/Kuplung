//
//  objParser2.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "objParser2.hpp"
#include "kuplung/utilities/helpers/Strings.h"
#include <filesystem>
#include <fstream>
#include <numeric>
#include <sstream>

objParser2::~objParser2() {}

void objParser2::init(const std::function<void(float)>& doProgress) {
  this->doProgress = doProgress;
  this->objFileLinesCount = 0;

  this->id_objTitle = "o ";
  this->id_geometricVertices = "v ";
  this->id_textureCoordinates = "vt ";
  this->id_vertexNormals = "vn ";
  this->id_spaceVertices = "vp ";
  this->id_face = "f ";
  this->id_materialFile = "mtllib ";
  this->id_useMaterial = "usemtl ";
  this->id_materialNew = "newmtl ";

  this->id_materialAmbientColor = "Ka ";
  this->id_materialDiffuseColor = "Kd ";
  this->id_materialSpecularColor = "Ks ";
  this->id_materialEmissionColor = "Ke ";
  this->id_materialSpecularExp = "Ns ";
  this->id_materialTransperant1 = "Tr ";
  this->id_materialTransperant2 = "d ";
  this->id_materialOpticalDensity = "Ni ";
  this->id_materialIllumination = "illum ";
  this->id_materialTextureAmbient = "map_Ka ";
  this->id_materialTextureDiffuse = "map_Kd ";
  this->id_materialTextureBump = "map_Bump ";
  this->id_materialTextureDisplacement = "disp ";
  this->id_materialTextureSpecular = "map_Ks ";
  this->id_materialTextureSpecularExp = "map_Ns ";
  this->id_materialTextureDissolve = "map_d ";

  this->parserUtils = std::make_unique<ParserUtils>();
}

std::vector<MeshModel> objParser2::parse(const FBEntity& fileToParse, const std::vector<std::string>& settings) {
  this->file = fileToParse;
  this->models = {};
  this->vectorVertices = {};
  this->vectorNormals = {};
  this->vectorTextureCoordinates = {};
  this->vectorIndices = {};

  std::ifstream ifs(this->file.path.c_str());
  if (!ifs.is_open()) {
    Settings::Instance()->funcDoLog("[objParser2] Cannot open .obj file" + this->file.path + "!");
    ifs.close();
    return {};
  }

  ifs.seekg(0);
  int progressStageTotal = int(std::count(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), '\n'));
  ifs.seekg(0);

  std::vector<unsigned int> indexModels, indexVertices, indexTexture, indexNormals;
  std::vector<glm::vec3> vVertices, vNormals;
  std::vector<glm::vec2> vTextureCoordinates;

  int modelCounter = 0, currentModelID = 0, progressStageCounter = 0;
  std::string singleLine;
  while (std::getline(ifs, singleLine)) {
    if (singleLine.starts_with(this->id_materialFile)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialFile), this->id_materialFile.length(), "");
      this->loadMaterialFile(singleLine);
    }
    else if (singleLine.starts_with(this->id_objTitle)) {
      currentModelID = modelCounter;
      MeshModel entityModel;
      entityModel.File = file;
      entityModel.ID = currentModelID;
      entityModel.ModelTitle = singleLine.replace(singleLine.find(this->id_objTitle), this->id_objTitle.length(), "");
      entityModel.countVertices = 0;
      entityModel.countTextureCoordinates = 0;
      entityModel.countNormals = 0;
      entityModel.countIndices = 0;
      modelCounter += 1;
      this->models.push_back(entityModel);
    }
    else if (singleLine.starts_with(this->id_geometricVertices)) {
      singleLine = singleLine.replace(singleLine.find(this->id_geometricVertices), this->id_geometricVertices.length(), "");
      std::stringstream valReader(singleLine);
      glm::vec3 v;
      valReader >> v.x >> v.y >> v.z;
      vVertices.push_back(v);
    }
    else if (singleLine.starts_with(this->id_textureCoordinates)) {
      singleLine = singleLine.replace(singleLine.find(this->id_textureCoordinates), this->id_textureCoordinates.length(), "");
      std::stringstream valReader(singleLine);
      glm::vec2 v;
      valReader >> v.x >> v.y;
      vTextureCoordinates.push_back(v);
    }
    else if (singleLine.starts_with(this->id_vertexNormals)) {
      singleLine = singleLine.replace(singleLine.find(this->id_vertexNormals), this->id_vertexNormals.length(), "");
      std::stringstream valReader(singleLine);
      glm::vec3 v;
      valReader >> v.x >> v.y >> v.z;
      vNormals.push_back(v);
    }
    else if (singleLine.starts_with(this->id_useMaterial)) {
      singleLine = singleLine.replace(singleLine.find(this->id_useMaterial), this->id_useMaterial.length(), "");
      this->models[static_cast<size_t>(currentModelID)].ModelMaterial = this->materials[singleLine];
      this->models[static_cast<size_t>(currentModelID)].MaterialTitle = this->models[static_cast<size_t>(currentModelID)].ModelMaterial.MaterialTitle;
    }
    else if (singleLine.starts_with(this->id_face)) {
      std::vector<std::string> ft = KuplungApp::Helpers::splitString(singleLine, ' ');
      if (ft.size() == 5) {
        unsigned int tri_vertexIndex[4], tri_uvIndex[4], tri_normalIndex[4];
        std::string face = this->id_face + "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d";
        int matches = std::sscanf(singleLine.c_str(), face.c_str(),
                                  &tri_vertexIndex[0], &tri_uvIndex[0], &tri_normalIndex[0],
                                  &tri_vertexIndex[1], &tri_uvIndex[1], &tri_normalIndex[1],
                                  &tri_vertexIndex[2], &tri_uvIndex[2], &tri_normalIndex[2],
                                  &tri_vertexIndex[3], &tri_uvIndex[3], &tri_normalIndex[3]);
        if (matches != 12) {
          face = this->id_face + "%d//%d %d//%d %d//%d %d//%d";
          matches = std::sscanf(singleLine.c_str(), face.c_str(),
                                &tri_vertexIndex[0], &tri_normalIndex[0],
                                &tri_vertexIndex[1], &tri_normalIndex[1],
                                &tri_vertexIndex[2], &tri_normalIndex[2],
                                &tri_vertexIndex[3], &tri_normalIndex[3]);
          if (matches != 8) {
            Settings::Instance()->funcDoLog("[objParser2] OBJ file is in wrong format!");
            return this->models;
          }
        }
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexVertices.push_back(tri_vertexIndex[0]);
        indexVertices.push_back(tri_vertexIndex[1]);
        indexVertices.push_back(tri_vertexIndex[2]);
        indexTexture.push_back(tri_uvIndex[0]);
        indexTexture.push_back(tri_uvIndex[1]);
        indexTexture.push_back(tri_uvIndex[2]);
        indexNormals.push_back(tri_normalIndex[0]);
        indexNormals.push_back(tri_normalIndex[1]);
        indexNormals.push_back(tri_normalIndex[2]);

        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexVertices.push_back(tri_vertexIndex[2]);
        indexVertices.push_back(tri_vertexIndex[3]);
        indexVertices.push_back(tri_vertexIndex[0]);
        indexTexture.push_back(tri_uvIndex[2]);
        indexTexture.push_back(tri_uvIndex[3]);
        indexTexture.push_back(tri_uvIndex[0]);
        indexNormals.push_back(tri_normalIndex[2]);
        indexNormals.push_back(tri_normalIndex[3]);
        indexNormals.push_back(tri_normalIndex[0]);
      }
      else {
        unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
        std::string face = this->id_face + "%d/%d/%d %d/%d/%d %d/%d/%d";
        int matches = std::sscanf(singleLine.c_str(), face.c_str(),
                                  &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                  &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                  &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
        if (matches != 9) {
          face = this->id_face + "%d//%d %d//%d %d//%d";
          matches = std::sscanf(singleLine.c_str(), face.c_str(),
                                &vertexIndex[0], &normalIndex[0],
                                &vertexIndex[1], &normalIndex[1],
                                &vertexIndex[2], &normalIndex[2]);
          if (matches != 6) {
            Settings::Instance()->funcDoLog("[objParser2] OBJ file is in wrong format!");
            return this->models;
          }
        }
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexModels.push_back(static_cast<unsigned int>(currentModelID));
        indexVertices.push_back(vertexIndex[0]);
        indexVertices.push_back(vertexIndex[1]);
        indexVertices.push_back(vertexIndex[2]);
        indexTexture.push_back(uvIndex[0]);
        indexTexture.push_back(uvIndex[1]);
        indexTexture.push_back(uvIndex[2]);
        indexNormals.push_back(normalIndex[0]);
        indexNormals.push_back(normalIndex[1]);
        indexNormals.push_back(normalIndex[2]);
      }
    }

    progressStageCounter += 1;
    float progress = (float(progressStageCounter) / float(progressStageTotal)) * 100.0f;
    this->doProgress(progress);
  }

  int Setting_Axis_Forward = 4;
  if (settings.size() > 0 && !settings[0].empty())
    Setting_Axis_Forward = std::stoi(settings[0]);
  int Setting_Axis_Up = 5;
  if (settings.size() > 1 && !settings[1].empty())
    Setting_Axis_Up = std::stoi(settings[1]);

  if (this->models.size() > 0) {
    progressStageCounter = 0;
    progressStageTotal = static_cast<int>(indexVertices.size());
    this->doProgress(0.0f);
    for (unsigned int i = 0; i < indexVertices.size(); i++) {
      unsigned int modelIndex = indexModels[i];
      unsigned int vertexIndex = indexVertices[i];
      unsigned int normalIndex = indexNormals[i];

      glm::vec3 vertex = this->parserUtils->fixVectorAxis(vVertices[vertexIndex - 1], Setting_Axis_Forward, Setting_Axis_Up);
      glm::vec3 normal = this->parserUtils->fixVectorAxis(vNormals[normalIndex - 1], Setting_Axis_Forward, Setting_Axis_Up);

      this->models[modelIndex].vertices.push_back(vertex);
      this->models[modelIndex].countVertices += 1;
      this->models[modelIndex].normals.push_back(normal);
      this->models[modelIndex].countNormals += 1;

      if (vTextureCoordinates.size() > 0) {
        unsigned int uvIndex = indexTexture[i];
        glm::vec2 uv = vTextureCoordinates[uvIndex - 1];
        this->models[modelIndex].texture_coordinates.push_back(uv);
        this->models[modelIndex].countTextureCoordinates += 1;
      }
      else
        this->models[modelIndex].countTextureCoordinates = 0;

      progressStageCounter += 1;
      float progress = (float(progressStageCounter) / float(progressStageTotal)) * 100.0f;
      this->doProgress(progress);
    }

    progressStageCounter = 0;
    progressStageTotal = static_cast<int>(this->models.size());
    this->doProgress(0.0f);
    std::map<PackedVertex, unsigned int> vertexToOutIndex;
    for (size_t i = 0; i < this->models.size(); i++) {
      MeshModel m = this->models[i];
      std::vector<glm::vec3> outVertices, outNormals;
      std::vector<glm::vec2> outTextureCoordinates;
      for (size_t j = 0; j < m.vertices.size(); j++) {
        PackedVertex packed = {m.vertices[j], (m.texture_coordinates.size() > 0) ? m.texture_coordinates[j] : glm::vec2(0.0f), m.normals[j]};

        unsigned int index;
        bool found = this->getSimilarVertexIndex(packed, vertexToOutIndex, index);
        if (found)
          m.indices.push_back(index);
        else {
          outVertices.push_back(m.vertices[j]);
          if (m.texture_coordinates.size() > 0)
            outTextureCoordinates.push_back(m.texture_coordinates[j]);
          outNormals.push_back(m.normals[j]);
          unsigned int newIndex = static_cast<unsigned int>(outVertices.size() - 1);
          m.indices.push_back(newIndex);
          vertexToOutIndex[packed] = newIndex;
        }
      }
      this->models[i].vertices = std::move(outVertices);
      this->models[i].texture_coordinates = std::move(outTextureCoordinates);
      this->models[i].normals = std::move(outNormals);
      this->models[i].indices = m.indices;
      this->models[i].countIndices = int(m.indices.size());

      progressStageCounter += 1;
      float progress = (float(progressStageCounter) / float(progressStageTotal)) * 100.0f;
      this->doProgress(progress);
    }
  }

  ifs.close();

  // Kuplung_printObjModels(this->models, false);

  return this->models;
}

bool objParser2::getSimilarVertexIndex(PackedVertex& packed, std::map<PackedVertex, unsigned int>& vertexToOutIndex, unsigned int& result) {
  std::map<PackedVertex, unsigned int>::iterator it = vertexToOutIndex.find(packed);
  if (it == vertexToOutIndex.end())
    return false;
  else {
    result = it->second;
    return true;
  }
}

void objParser2::loadMaterialFile(const std::string& materialFile) {
  this->materials.clear();

  std::string materialPath = this->file.path.substr(0, this->file.path.find_last_of("\\/")) + "/" + materialFile;
  std::ifstream ifs(materialPath.c_str());
  if (!ifs.is_open()) {
    Settings::Instance()->funcDoLog("[objParser2] Cannot open .mtl file - " + materialPath + "!");
    return;
  }

  int MaterialID = 0;
  std::string singleLine, currentMaterialTitle;
  while (std::getline(ifs, singleLine)) {
    if (singleLine.starts_with(this->id_materialNew)) {
      currentMaterialTitle = singleLine.replace(singleLine.find(this->id_materialNew), this->id_materialNew.length(), "");
      MeshModelMaterial entityMaterial = {};
      entityMaterial.MaterialID = MaterialID;
      entityMaterial.MaterialTitle = currentMaterialTitle;
      entityMaterial.SpecularExp = 1.0;
      entityMaterial.Transparency = 1.0;
      entityMaterial.IlluminationMode = 2;
      entityMaterial.OpticalDensity = 1.0;
      entityMaterial.AmbientColor = glm::vec3(0.0f);
      entityMaterial.DiffuseColor = glm::vec3(0.0f);
      entityMaterial.SpecularColor = glm::vec3(0.0f);
      entityMaterial.EmissionColor = glm::vec3(0.0f);
      MaterialID += 1;
      this->materials[currentMaterialTitle] = entityMaterial;
    }
    else if (singleLine.starts_with(this->id_materialAmbientColor)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialAmbientColor), this->id_materialAmbientColor.length(), "");
      std::stringstream valReader(singleLine);
      glm::vec3 v;
      valReader >> v.x >> v.y >> v.z;
      this->materials[currentMaterialTitle].AmbientColor = v;
    }
    else if (singleLine.starts_with(this->id_materialDiffuseColor)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialDiffuseColor), this->id_materialDiffuseColor.length(), "");
      std::stringstream valReader(singleLine);
      glm::vec3 v;
      valReader >> v.x >> v.y >> v.z;
      this->materials[currentMaterialTitle].DiffuseColor = v;
    }
    else if (singleLine.starts_with(this->id_materialSpecularColor)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialSpecularColor), this->id_materialSpecularColor.length(), "");
      std::stringstream valReader(singleLine);
      glm::vec3 v;
      valReader >> v.x >> v.y >> v.z;
      this->materials[currentMaterialTitle].SpecularColor = v;
    }
    else if (singleLine.starts_with(this->id_materialEmissionColor)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialEmissionColor), this->id_materialEmissionColor.length(), "");
      std::stringstream valReader(singleLine);
      glm::vec3 v;
      valReader >> v.x >> v.y >> v.z;
      this->materials[currentMaterialTitle].EmissionColor = v;
    }
    else if (singleLine.starts_with(this->id_materialSpecularExp)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialSpecularExp), this->id_materialSpecularExp.length(), "");
      this->materials[currentMaterialTitle].SpecularExp = std::stof(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTransperant1) || singleLine.starts_with(this->id_materialTransperant2)) {
      if (singleLine.starts_with(this->id_materialTransperant1))
        singleLine = singleLine.replace(singleLine.find(this->id_materialTransperant1), this->id_materialTransperant1.length(), "");
      else
        singleLine = singleLine.replace(singleLine.find(this->id_materialTransperant2), this->id_materialTransperant2.length(), "");
      this->materials[currentMaterialTitle].Transparency = std::stof(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialOpticalDensity)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialOpticalDensity), this->id_materialOpticalDensity.length(), "");
      this->materials[currentMaterialTitle].OpticalDensity = std::stof(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialIllumination)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialIllumination), this->id_materialIllumination.length(), "");
      this->materials[currentMaterialTitle].IlluminationMode = std::stoi(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTextureAmbient)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialTextureAmbient), this->id_materialTextureAmbient.length(), "");
      this->materials[currentMaterialTitle].TextureAmbient = this->parseTextureImage(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTextureBump)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialTextureBump), this->id_materialTextureBump.length(), "");
      this->materials[currentMaterialTitle].TextureBump = this->parseTextureImage(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTextureDiffuse)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialTextureDiffuse), this->id_materialTextureDiffuse.length(), "");
      this->materials[currentMaterialTitle].TextureDiffuse = this->parseTextureImage(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTextureDisplacement)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialTextureDisplacement), this->id_materialTextureDisplacement.length(), "");
      this->materials[currentMaterialTitle].TextureDisplacement = this->parseTextureImage(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTextureDissolve)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialTextureDissolve), this->id_materialTextureDissolve.length(), "");
      this->materials[currentMaterialTitle].TextureDissolve = this->parseTextureImage(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTextureSpecular)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialTextureSpecular), this->id_materialTextureSpecular.length(), "");
      this->materials[currentMaterialTitle].TextureSpecular = this->parseTextureImage(singleLine);
    }
    else if (singleLine.starts_with(this->id_materialTextureSpecularExp)) {
      singleLine = singleLine.replace(singleLine.find(this->id_materialTextureSpecularExp), this->id_materialTextureSpecularExp.length(), "");
      this->materials[currentMaterialTitle].TextureSpecularExp = this->parseTextureImage(singleLine);
    }
  }
}

MeshMaterialTextureImage objParser2::parseTextureImage(const std::string& textureLine) {
  MeshMaterialTextureImage materialImage;

  materialImage.Height = 0;
  materialImage.Width = 0;
  materialImage.UseTexture = true;

  if (textureLine.find('-') != std::string::npos) {
    std::vector<std::string> lineElements = KuplungApp::Helpers::splitString(textureLine, '-');

    if (lineElements[0].empty())
      lineElements.erase(lineElements.begin());

    std::vector<std::string> lastElements = KuplungApp::Helpers::splitString(lineElements[lineElements.size() - 1], ' ');
    materialImage.Image = lastElements[lastElements.size() - 1];
    lastElements.erase(lastElements.end() - 1);

    std::ostringstream lastCommandsOS;
    copy(lastElements.begin(), lastElements.end(), std::ostream_iterator<std::string>(lastCommandsOS, " "));
    std::string lastCommand = lastCommandsOS.str();
    lineElements[lineElements.size() - 1] = lastCommand;

    for (size_t i = 0; i < lineElements.size(); i++) {
      materialImage.Commands.push_back("-" + lineElements[i]);
    }
  }
  else
    materialImage.Image = textureLine;
  materialImage.Image = KuplungApp::Helpers::trim(materialImage.Image);

  std::string folderPath = this->file.path;
  if (folderPath.find(this->file.title) != std::string::npos)
    folderPath = folderPath.replace(folderPath.find(this->file.title), this->file.title.length(), "");
  if (!std::filesystem::exists(materialImage.Image) && !std::filesystem::path(materialImage.Image).is_absolute())
    materialImage.Image = folderPath + materialImage.Image;

  std::vector<std::string> fileElements = KuplungApp::Helpers::splitString(materialImage.Image, '/');
  materialImage.Filename = fileElements[fileElements.size() - 1];
  return materialImage;
}
