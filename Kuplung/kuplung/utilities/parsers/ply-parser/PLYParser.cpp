//
//  PLYParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "PLYParser.hpp"
#include "kuplung/utilities/helpers/Strings.h"
#include <fstream>
#include <sstream>

PLYParser::PLYParser() {
  this->Setting_Axis_Forward = 0;
  this->Setting_Axis_Up = 0;
}

PLYParser::~PLYParser() {}

void PLYParser::init(const std::function<void(float)>& doProgress) {
  this->funcProgress = doProgress;
}

std::vector<MeshModel> PLYParser::parse(const FBEntity& file, const std::vector<std::string>& settings) {
  if (settings.size() > 0 && !settings[0].empty())
    this->Setting_Axis_Forward = std::stoi(settings[0]);
  if (settings.size() > 1 && !settings[1].empty())
    this->Setting_Axis_Up = std::stoi(settings[1]);

  std::ifstream ply_file_data(file.path.c_str());
  if (!ply_file_data.is_open()) {
    Settings::Instance()->funcDoLog("[PLYParser] Cannot open .ply file - " + file.path + "!");
    ply_file_data.close();
    return {};
  }

  std::string singleLine;
  ply_file_data.seekg(0);
  std::getline(ply_file_data, singleLine);
  if (singleLine != "ply" && singleLine != "PLY") {
    Settings::Instance()->funcDoLog("[PLYParser] File is not in PLY format - " + file.path + "!");
    ply_file_data.close();
    return {};
  }
  ply_file_data.seekg(0);

  while (std::getline(ply_file_data, singleLine)) {
    if (singleLine.starts_with("format")) {
      singleLine = singleLine.replace(singleLine.find("format "), 7, "");
      if (singleLine.starts_with("ascii")) {
        this->loadAsciiFile(file);
        break;
      }
    }
  }

  ply_file_data.close();

  return this->models;
}

void PLYParser::loadAsciiFile(const FBEntity& file) {
  Settings::Instance()->funcDoLog("[PLYParser] Parsing ascii PLY file...");

  int currentModelID = 0;
  MeshModel entityModel;
  entityModel.File = file;
  entityModel.ID = currentModelID;
  entityModel.ModelTitle = file.title;
  entityModel.countVertices = 0;
  entityModel.countTextureCoordinates = 0;
  entityModel.countNormals = 0;
  entityModel.countIndices = 0;

  MeshModelMaterial entityMaterial = {};
  entityMaterial.MaterialID = 1;
  entityMaterial.MaterialTitle = "<PLY-MAT>";
  entityMaterial.DiffuseColor = glm::vec3(0.7, 0.7, 0.7);
  entityMaterial.Transparency = 1.0f;
  entityMaterial.IlluminationMode = 2;
  entityModel.ModelMaterial = entityMaterial;

  entityModel.countIndices = int(entityModel.indices.size());
  entityModel.countNormals = int(entityModel.normals.size());
  entityModel.countVertices = int(entityModel.vertices.size());

  unsigned int countVertices = 0, counterVertices = 0;
  unsigned int countFaces = 1, counterFaces = 1;
  bool inElementVertex = false;
  std::map<std::string, bool> ply_props;

  std::ifstream ply_file_data(file.path.c_str());
  std::string singleLine;
  bool headerEnded = false;
  while (std::getline(ply_file_data, singleLine)) {
    // elements
    if (singleLine.starts_with("element")) {
      singleLine = singleLine.replace(singleLine.find("element "), 8, "");

      // vertices
      if (singleLine.starts_with("vertex")) {
        singleLine = singleLine.replace(singleLine.find("vertex "), 7, "");
        countVertices = std::stoi(singleLine);
        inElementVertex = true;
      }

      // faces
      if (singleLine.starts_with("face")) {
        singleLine = singleLine.replace(singleLine.find("face "), 5, "");
        countFaces = std::stoi(singleLine);
        inElementVertex = false;
      }

      // Face properties are not supported ... too unclear for me still.
    }

    // vertex properties
    if (singleLine.starts_with("property") && inElementVertex) {
      singleLine = singleLine.replace(singleLine.find("property "), 9, "");
      std::stringstream valReader(singleLine);
      std::string prop_type, prop_value;
      valReader >> prop_type >> prop_value;
      if (prop_value == "nx")
        ply_props["normals"] = true;
      else if (prop_value == "red")
        ply_props["colors"] = true;
      else if (prop_value == "s")
        ply_props["textures"] = true;
    }

    // data
    if (headerEnded) {
      if (counterVertices < countVertices) {
        std::stringstream valReader(singleLine);

        glm::vec3 vertex = glm::vec3(0.0);
        valReader >> vertex.x >> vertex.y >> vertex.z;
        entityModel.vertices.push_back(vertex);

        if (ply_props["normals"]) {
          glm::vec3 normal = glm::vec3(0.0);
          valReader >> normal.x >> normal.y >> normal.z;
          entityModel.normals.push_back(normal);
        }

        if (ply_props["textures"]) {
          glm::vec2 tc = glm::vec2(0.0);
          valReader >> tc.s >> tc.t;
          entityModel.texture_coordinates.push_back(tc);
        }

        if (ply_props["colors"]) {
          glm::vec3 color = glm::vec3(0.0);
          valReader >> color.r >> color.g >> color.b;
          entityMaterial.DiffuseColor = color;
        }

        counterVertices += 1;
      }
      if (counterVertices >= countVertices && counterFaces < countFaces) {
        std::stringstream valReader(singleLine);
        int fCount;
        valReader >> fCount;
        if (fCount == 3) {
          unsigned int index1, index2, index3;
          valReader >> index1 >> index2 >> index3;
          entityModel.indices.push_back(index1);
          entityModel.indices.push_back(index2);
          entityModel.indices.push_back(index3);
        }
        else if (fCount == 4) {
          unsigned int index1, index2, index3, index4;
          valReader >> index1 >> index2 >> index3 >> index4;
          entityModel.indices.push_back(index1);
          entityModel.indices.push_back(index2);
          entityModel.indices.push_back(index3);
          entityModel.indices.push_back(index4);
        }

        counterFaces += 1;
      }
    }

    if (singleLine.starts_with("end_header"))
      headerEnded = true;
  }
  ply_file_data.close();

  entityModel.ModelMaterial = entityMaterial;
  entityModel.countVertices = int(entityModel.vertices.size()) * 3;
  entityModel.countTextureCoordinates = int(entityModel.texture_coordinates.size()) * 2;
  entityModel.countNormals = int(entityModel.normals.size()) * 3;
  entityModel.countIndices = int(entityModel.indices.size());

  currentModelID += 1;
  this->models.push_back(entityModel);

  Kuplung_printObjModels(this->models, false);
}
