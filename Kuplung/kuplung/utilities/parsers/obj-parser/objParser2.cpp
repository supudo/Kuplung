//
//  objParser2.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "objParser2.hpp"
#include <fstream>
#include <numeric>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

objParser2::~objParser2() {
}

void objParser2::init(std::function<void(float)> doProgress) {
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

std::vector<MeshModel> objParser2::parse(const FBEntity& file, const std::vector<std::string>& settings) {
    this->file = file;
    this->models = {};
    this->vectorVertices = {};
    this->vectorNormals = {};
    this->vectorTextureCoordinates = {};
    this->vectorIndices = {};

    std::ifstream ifs(this->file.path.c_str());
    if (!ifs.is_open()) {
        Settings::Instance()->funcDoLog("Cannot open .obj file" + this->file.path + "!");
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
        if (boost::starts_with(singleLine, this->id_materialFile)) {
            boost::replace_first(singleLine, this->id_materialFile, "");
            this->loadMaterialFile(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_objTitle)) {
            currentModelID = modelCounter;
            MeshModel entityModel;
            entityModel.File = file;
            entityModel.ID = currentModelID;
            entityModel.ModelTitle = singleLine;
            boost::replace_first(entityModel.ModelTitle, this->id_objTitle, "");
            entityModel.countVertices = 0;
            entityModel.countTextureCoordinates = 0;
            entityModel.countNormals = 0;
            entityModel.countIndices = 0;
            modelCounter += 1;
            this->models.push_back(entityModel);
        }
        else if (boost::starts_with(singleLine, this->id_geometricVertices)) {
            boost::replace_first(singleLine, this->id_geometricVertices, "");
            std::stringstream valReader(singleLine);
            glm::vec3 v;
            valReader >> v.x >> v.y >> v.z;
            vVertices.push_back(v);
        }
        else if (boost::starts_with(singleLine, this->id_textureCoordinates)) {
            boost::replace_first(singleLine, this->id_textureCoordinates, "");
            std::stringstream valReader(singleLine);
            glm::vec2 v;
            valReader >> v.x >> v.y;
            vTextureCoordinates.push_back(v);
        }
        else if (boost::starts_with(singleLine, this->id_vertexNormals)) {
            boost::replace_first(singleLine, this->id_vertexNormals, "");
            std::stringstream valReader(singleLine);
            glm::vec3 v;
            valReader >> v.x >> v.y >> v.z;
            vNormals.push_back(v);
        }
        else if (boost::starts_with(singleLine, this->id_useMaterial)) {
            boost::replace_first(singleLine, this->id_useMaterial, "");
            this->models[static_cast<size_t>(currentModelID)].ModelMaterial = this->materials[singleLine];
            this->models[static_cast<size_t>(currentModelID)].MaterialTitle = this->models[static_cast<size_t>(currentModelID)].ModelMaterial.MaterialTitle;
        }
        else if (boost::starts_with(singleLine, this->id_face)) {
            std::vector<std::string> ft = this->splitString(singleLine, " ");
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
                        Settings::Instance()->funcDoLog("OBJ file is in wrong format!");
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
                        Settings::Instance()->funcDoLog("OBJ file is in wrong format!");
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
    if (settings.size() > 0 && settings[0] != "")
        Setting_Axis_Forward = std::stoi(settings[0]);
    int Setting_Axis_Up = 5;
    if (settings.size() > 1 && settings[1] != "")
        Setting_Axis_Up = std::stoi(settings[1]);

    if (this->models.size() > 0) {
        progressStageCounter = 0;
        progressStageTotal = static_cast<int>(indexVertices.size());
        this->doProgress(0.0f);
        for (unsigned int i=0; i<indexVertices.size(); i++) {
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
        for (size_t i=0; i<this->models.size(); i++) {
            MeshModel m = this->models[i];
            std::vector<glm::vec3> outVertices, outNormals;
            std::vector<glm::vec2> outTextureCoordinates;
            for (size_t j=0; j<m.vertices.size(); j++) {
                PackedVertex packed = { m.vertices[j], (m.texture_coordinates.size() > 0) ? m.texture_coordinates[j] : glm::vec2(0.0f), m.normals[j] };

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
            this->models[i].vertices = outVertices;
            this->models[i].texture_coordinates = outTextureCoordinates;
            this->models[i].normals = outNormals;
            this->models[i].indices = m.indices;
            this->models[i].countIndices = int(m.indices.size());

            progressStageCounter += 1;
            float progress = (float(progressStageCounter) / float(progressStageTotal)) * 100.0f;
            this->doProgress(progress);
        }
    }

    ifs.close();

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
        Settings::Instance()->funcDoLog("Cannot open .mtl file - " + materialPath + "!");
        return;
    }

    int MaterialID = 0;
    std::string singleLine, currentMaterialTitle;
    while (std::getline(ifs, singleLine)) {
        if (boost::starts_with(singleLine, this->id_materialNew)) {
            currentMaterialTitle = singleLine;
            boost::replace_first(currentMaterialTitle, this->id_materialNew, "");
            MeshModelMaterial entityMaterial = {};
            entityMaterial.MaterialID = MaterialID;
            entityMaterial.MaterialTitle = currentMaterialTitle;
            entityMaterial.SpecularExp = -1.0;
            entityMaterial.Transparency = -1.0;
            entityMaterial.IlluminationMode = -1.0;
            entityMaterial.OpticalDensity = -1.0;
            entityMaterial.AmbientColor = glm::vec3(0.0f);
            entityMaterial.DiffuseColor = glm::vec3(0.0f);
            entityMaterial.SpecularColor = glm::vec3(0.0f);
            entityMaterial.EmissionColor = glm::vec3(0.0f);;
            MaterialID += 1;
            this->materials[currentMaterialTitle] = entityMaterial;
        }
        else if (boost::starts_with(singleLine, this->id_materialAmbientColor)) {
            boost::replace_first(singleLine, this->id_materialAmbientColor, "");
            std::stringstream valReader(singleLine);
            glm::vec3 v;
            valReader >> v.x >> v.y >> v.z;
            this->materials[currentMaterialTitle].AmbientColor = v;
        }
        else if (boost::starts_with(singleLine, this->id_materialDiffuseColor)) {
            boost::replace_first(singleLine, this->id_materialDiffuseColor, "");
            std::stringstream valReader(singleLine);
            glm::vec3 v;
            valReader >> v.x >> v.y >> v.z;
            this->materials[currentMaterialTitle].DiffuseColor = v;
        }
        else if (boost::starts_with(singleLine, this->id_materialSpecularColor)) {
            boost::replace_first(singleLine, this->id_materialSpecularColor, "");
            std::stringstream valReader(singleLine);
            glm::vec3 v;
            valReader >> v.x >> v.y >> v.z;
            this->materials[currentMaterialTitle].SpecularColor = v;
        }
        else if (boost::starts_with(singleLine, this->id_materialEmissionColor)) {
            boost::replace_first(singleLine, this->id_materialEmissionColor, "");
            std::stringstream valReader(singleLine);
            glm::vec3 v;
            valReader >> v.x >> v.y >> v.z;
            this->materials[currentMaterialTitle].EmissionColor = v;
        }
        else if (boost::starts_with(singleLine, this->id_materialSpecularExp)) {
            boost::replace_first(singleLine, this->id_materialSpecularExp, "");
            this->materials[currentMaterialTitle].SpecularExp = std::stof(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTransperant1) || boost::starts_with(singleLine, this->id_materialTransperant2)) {
            if (boost::starts_with(singleLine, this->id_materialTransperant1))
                boost::replace_first(singleLine, this->id_materialTransperant1, "");
            else
                boost::replace_first(singleLine, this->id_materialTransperant2, "");
            this->materials[currentMaterialTitle].Transparency = std::stof(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialOpticalDensity)) {
            boost::replace_first(singleLine, this->id_materialOpticalDensity, "");
            this->materials[currentMaterialTitle].OpticalDensity = std::stof(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialIllumination)) {
            boost::replace_first(singleLine, this->id_materialIllumination, "");
            this->materials[currentMaterialTitle].IlluminationMode = std::stoi(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTextureAmbient)) {
            boost::replace_first(singleLine, this->id_materialTextureAmbient, "");
            this->materials[currentMaterialTitle].TextureAmbient = this->parseTextureImage(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTextureBump)) {
            boost::replace_first(singleLine, this->id_materialTextureBump, "");
            this->materials[currentMaterialTitle].TextureBump = this->parseTextureImage(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTextureDiffuse)) {
            boost::replace_first(singleLine, this->id_materialTextureDiffuse, "");
            this->materials[currentMaterialTitle].TextureDiffuse = this->parseTextureImage(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTextureDisplacement)) {
            boost::replace_first(singleLine, this->id_materialTextureDisplacement, "");
            this->materials[currentMaterialTitle].TextureDisplacement = this->parseTextureImage(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTextureDissolve)) {
            boost::replace_first(singleLine, this->id_materialTextureDissolve, "");
            this->materials[currentMaterialTitle].TextureDissolve = this->parseTextureImage(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTextureSpecular)) {
            boost::replace_first(singleLine, this->id_materialTextureSpecular, "");
            this->materials[currentMaterialTitle].TextureSpecular = this->parseTextureImage(singleLine);
        }
        else if (boost::starts_with(singleLine, this->id_materialTextureSpecularExp)) {
            boost::replace_first(singleLine, this->id_materialTextureSpecularExp, "");
            this->materials[currentMaterialTitle].TextureSpecularExp = this->parseTextureImage(singleLine);
        }
    }
}

MeshMaterialTextureImage objParser2::parseTextureImage(const std::string& textureLine) {
    MeshMaterialTextureImage materialImage;

    materialImage.Height = 0;
    materialImage.Width = 0;
    materialImage.UseTexture = true;

    if (textureLine.find("-") != std::string::npos) {
        std::vector<std::string> lineElements = this->splitString(textureLine, "-");

        if (lineElements[0].empty())
            lineElements.erase(lineElements.begin());

        std::vector<std::string> lastElements = this->splitString(lineElements[lineElements.size() - 1], " ");
        materialImage.Image = lastElements[lastElements.size() - 1];
        lastElements.erase(lastElements.end() - 1);

        std::ostringstream lastCommandsOS;
        copy(lastElements.begin(), lastElements.end(), std::ostream_iterator<std::string>(lastCommandsOS, " "));
        std::string lastCommand = lastCommandsOS.str();
        lineElements[lineElements.size() - 1] = lastCommand;

        for (size_t i=0; i<lineElements.size(); i++) {
            materialImage.Commands.push_back("-" + lineElements[i]);
        }
    }
    else
        materialImage.Image = textureLine;
    boost::algorithm::trim(materialImage.Image);

    std::string folderPath = this->file.path;
    boost::replace_all(folderPath, this->file.title, "");
    if (!boost::filesystem::exists(materialImage.Image) && !boost::filesystem::path(materialImage.Image).is_absolute())
        materialImage.Image = folderPath + materialImage.Image;

    std::vector<std::string> fileElements = this->splitString(materialImage.Image, "/");
    materialImage.Filename = fileElements[fileElements.size() - 1];
    return materialImage;
}

std::vector<std::string> objParser2::splitString(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> elements;
    boost::split(elements, s, boost::is_any_of(delimiter));
    return elements;
}
