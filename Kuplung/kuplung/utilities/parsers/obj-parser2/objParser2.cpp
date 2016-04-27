//
//  objParser3.cpp
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
#include <boost/algorithm/string.hpp>

#pragma mark - Destructor

objParser2::~objParser2() {
    this->destroy();
}

#pragma mark - Publics

void objParser2::init(std::function<void(float)> doProgress) {
    this->scene = {};
    this->doProgress = doProgress;
    this->objFileLinesCount = 0;

    this->regex_comment = "^#.*";
    this->regex_whiteSpace = "\\s";

    this->regex_objTitle = "^o.*";
    this->regex_geometricVertices = "^v\\s.*";
    this->regex_textureCoordinates = "^vt\\s.*";
    this->regex_vertexNormals = "^vn\\s.*";
    this->regex_spaceVertices = "^vp\\s.*";
    this->regex_polygonalFaces = "^f\\s.*";
    this->regex_polygonalFacesSingle = "[/\\\\]";
    this->regex_materialFile = "^mtllib\\s.*";
    this->regex_useMaterial = "^usemtl\\s.*";
    this->regex_materialNew = "^newmtl\\s.*";

    this->regex_materialAmbientColor = "^Ka\\s.*";
    this->regex_materialDiffuseColor = "^Kd\\s.*";
    this->regex_materialSpecularColor = "^Ks\\s.*";
    this->regex_materialEmissionColor = "^Ke\\s.*";
    this->regex_materialSpecularExp = "^Ns\\s.*";
    this->regex_materialTransperant1 = "^Tr\\s.*";
    this->regex_materialTransperant2 = "^d\\s.*";
    this->regex_materialOpticalDensity = "^Ni\\s.*";
    this->regex_materialIllumination = "^illum\\s.*";
    this->regex_materialTextureAmbient = "^map_Ka\\s.*";
    this->regex_materialTextureDiffuse = "^map_Kd\\s.*";
    this->regex_materialTextureBump = "^map_Bump\\s.*";
    this->regex_materialTextureDisplacement = "^disp\\s.*";
    this->regex_materialTextureSpecular = "^map_Ks\\s.*";
    this->regex_materialTextureSpecularExp = "^map_Ns\\s.*";
    this->regex_materialTextureDissolve = "^map_d\\s.*";
}

void objParser2::destroy() {
}

objScene objParser2::parse(FBEntity file) {
    this->file = file;
    this->geometricVertices = {};
    this->textureCoordinates = {};
    this->vertexNormals = {};
    this->spaceVertices = {};
    this->polygonalFaces = {};
    this->vectorVertices = {};
    this->vectorNormals = {};
    this->vectorTextureCoordinates = {};
    this->vectorIndices = {};
    this->scene = {};
    this->scene.totalCountGeometricVertices = 0;
    this->scene.totalCountTextureCoordinates = 0;
    this->scene.totalCountNormalVertices = 0;
    this->scene.totalCountIndices = 0;
    this->scene.totalCountFaces = 0;

    std::FILE *fp = std::fopen(this->file.path.c_str(), "rb");
    if (fp) {
        std::string fileContents;
        std::fseek(fp, 0, SEEK_END);
        fileContents.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&fileContents[0], 1, fileContents.size(), fp);
        std::fclose(fp);

        this->scene.objFile = this->file.title;

        size_t pos = 0;
        std::string singleLine;

        int indexModel = -1, indexFace = -1;
        int indicesCounter = 0;

        this->objFileLinesCount = this->getLineCount();
        int linesProcessedCounter = 0;

        int modelID = 1, faceID = 1;
        while ((pos = fileContents.find(Settings::Instance()->newLineDelimiter)) != std::string::npos) {
            singleLine = fileContents.substr(0, pos);

            if (singleLine == "" || std::regex_match(singleLine, this->regex_comment)) {
                fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
                continue;
            }

            std::vector<std::string> lineElements = this->splitString(singleLine, this->regex_whiteSpace);
            lineElements.erase(lineElements.begin());

            if (std::regex_match(singleLine, this->regex_objTitle) || std::regex_match(singleLine, this->regex_materialFile)) {
                if (std::regex_match(singleLine, this->regex_objTitle)) {
                    objModel entityModel;
                    entityModel.ID = modelID;
                    entityModel.modelID = std::accumulate(begin(lineElements), end(lineElements), entityModel.modelID);
                    entityModel.verticesCount = 0;
                    entityModel.textureCoordinatesCount = 0;
                    entityModel.normalsCount = 0;
                    entityModel.indicesCount = 0;
                    indexModel += 1;
                    indexFace = -1;
                    modelID += 1;
                    this->scene.models.push_back(entityModel);
                }
            }

            if (std::regex_match(singleLine, this->regex_materialFile))
                this->scene.materials = this->loadMaterial(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_geometricVertices)) {
                std::vector<float> points = this->string2float(lineElements);
                this->geometricVertices.insert(end(this->geometricVertices), begin(points), end(points));
            }
            else if (std::regex_match(singleLine, this->regex_textureCoordinates)) {
                std::vector<float> points = this->string2float(lineElements);
                this->textureCoordinates.insert(end(this->textureCoordinates), begin(points), end(points));
            }
            else if (std::regex_match(singleLine, this->regex_vertexNormals)) {
                std::vector<float> points = this->string2float(lineElements);
                this->vertexNormals.insert(end(this->vertexNormals), begin(points), end(points));
            }
            else if (std::regex_match(singleLine, this->regex_spaceVertices)) {
                std::vector<float> points = this->string2float(lineElements);
                this->spaceVertices.insert(end(this->spaceVertices), begin(points), end(points));
            }
            else if (std::regex_match(singleLine, this->regex_useMaterial)) {
                objModelFace entityFace;
                entityFace.ID = faceID;
                entityFace.ModelTitle = this->scene.models[indexModel].modelID;
                entityFace.objFile = this->scene.objFile;
                entityFace.ModelID = this->scene.models[indexModel].ID;
                entityFace.materialID = std::accumulate(begin(lineElements), end(lineElements), entityFace.materialID);
                entityFace.verticesCount = 0;
                entityFace.textureCoordinatesCount = 0;
                entityFace.normalsCount = 0;
                entityFace.indicesCount = 0;
                indicesCounter = 0;
                indexFace += 1;
                faceID += 1;
                this->scene.models[indexModel].faces.push_back(entityFace);
                this->scene.totalCountFaces += 1;
            }
            else if (std::regex_match(singleLine, this->regex_polygonalFaces)) {
                std::vector<std::string> singleFaceElements = this->splitString(singleLine, this->regex_whiteSpace);
                singleFaceElements.erase(singleFaceElements.begin());


                for (size_t i=0; i<singleFaceElements.size(); i++) {
                    std::vector<std::string> face = this->splitString(singleFaceElements[i], this->regex_polygonalFacesSingle);

                    int v_idx = (std::stoi(face[0]) - 1) * 3;
                    this->scene.models[indexModel].faces[indexFace].vertices.push_back(this->geometricVertices[v_idx + 0]);
                    this->scene.models[indexModel].faces[indexFace].vertices.push_back(this->geometricVertices[v_idx + 1]);
                    this->scene.models[indexModel].faces[indexFace].vertices.push_back(this->geometricVertices[v_idx + 2]);
                    this->scene.totalCountGeometricVertices += 3;
                    this->scene.models[indexModel].verticesCount += 3;
                    this->scene.models[indexModel].faces[indexFace].verticesCount += 3;
                    glm::vec3 v_v = glm::vec3(this->geometricVertices[v_idx + 0], this->geometricVertices[v_idx + 1], this->geometricVertices[v_idx + 2]);
                    this->scene.models[indexModel].faces[indexFace].vectors_vertices.push_back(v_v);
                    this->vectorVertices.push_back(v_v);

                    if (this->textureCoordinates.size() > 0 && face[1] != "") {
                        int t_idx = (std::stoi(face[1]) - 1) * 2;
                        this->scene.models[indexModel].faces[indexFace].texture_coordinates.push_back(this->textureCoordinates[t_idx + 0]);
                        this->scene.models[indexModel].faces[indexFace].texture_coordinates.push_back(this->textureCoordinates[t_idx + 1]);
                        this->scene.totalCountTextureCoordinates += 2;
                        this->scene.models[indexModel].textureCoordinatesCount += 2;
                        this->scene.models[indexModel].faces[indexFace].textureCoordinatesCount += 2;
                        glm::vec2 v_tc = glm::vec2(this->textureCoordinates[t_idx + 0], this->textureCoordinates[t_idx + 1]);
                        this->scene.models[indexModel].faces[indexFace].vectors_texture_coordinates.push_back(v_tc);
                        this->vectorTextureCoordinates.push_back(v_tc);
                    }
                    else {
                        this->scene.models[indexModel].faces[indexFace].vectors_texture_coordinates.push_back(glm::vec2(0.0f));
                        this->vectorTextureCoordinates.push_back(glm::vec2(0.0f));
                    }

                    int n_idx = (std::stoi(face[2]) - 1) * 3;
                    this->scene.models[indexModel].faces[indexFace].normals.push_back(this->vertexNormals[n_idx + 0]);
                    this->scene.models[indexModel].faces[indexFace].normals.push_back(this->vertexNormals[n_idx + 1]);
                    this->scene.models[indexModel].faces[indexFace].normals.push_back(this->vertexNormals[n_idx + 2]);
                    this->scene.totalCountNormalVertices += 3;
                    this->scene.models[indexModel].normalsCount += 3;
                    this->scene.models[indexModel].faces[indexFace].normalsCount += 3;
                    glm::vec3 v_n = glm::vec3(this->vertexNormals[n_idx + 0], this->vertexNormals[n_idx + 1], this->vertexNormals[n_idx + 2]);
                    this->scene.models[indexModel].faces[indexFace].vectors_normals.push_back(v_n);
                    this->vectorNormals.push_back(v_n);

                    //this->scene.models[indexModel].faces[indexFace].indices.push_back(indicesCounter);
                    //indicesCounter += 1;
                    //this->scene.totalCountIndices += 1;
                    //this->scene.models[indexModel].faces[indexFace].indicesCount += 1;
                    //this->scene.models[indexModel].indicesCount += 1;

                    this->scene.models[indexModel].faces[indexFace].solidColor.push_back(1.0);
                    this->scene.models[indexModel].faces[indexFace].solidColor.push_back(0.0);
                    this->scene.models[indexModel].faces[indexFace].solidColor.push_back(0.0);
                }
            }

            linesProcessedCounter += 1;
            float progress = ((float)linesProcessedCounter / (float)this->objFileLinesCount) * 100.0;
            this->doProgress(progress);
            fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
        }
    }

    this->scene.totalCountIndices = 0;
    for (size_t i=0; i<this->scene.models.size(); i++) {
        this->scene.models[i].indicesCount = 0;
        for (size_t j=0; j<this->scene.models[i].faces.size(); j++) {
            std::vector<unsigned int> indices;
            std::vector<glm::vec3> indexed_vertices;
            std::vector<glm::vec2> indexed_uvs;
            std::vector<glm::vec3> indexed_normals;

            objModelFace oFace = this->scene.models[i].faces[j];
            this->fixIndices(
                        oFace.vectors_vertices,
                        oFace.vectors_texture_coordinates,
                        oFace.vectors_normals,
                        indices,
                        indexed_vertices,
                        indexed_uvs,
                        indexed_normals
            );
            this->scene.models[i].faces[j].vectors_vertices = indexed_vertices;
            this->scene.models[i].faces[j].vectors_texture_coordinates = indexed_uvs;
            this->scene.models[i].faces[j].vectors_normals = indexed_normals;
            this->scene.models[i].faces[j].indices = indices;
            this->scene.models[i].faces[j].verticesCount = (int)indexed_vertices.size();
            this->scene.models[i].faces[j].indicesCount = (int)indices.size();
            this->scene.models[i].indicesCount += this->scene.models[i].faces[j].indicesCount;
        }
    }

    for (size_t i=0; i<this->scene.models.size(); i++) {
        for (size_t j=0; j<this->scene.models[i].faces.size(); j++) {
            this->scene.models[i].faces[j].faceMaterial = this->findMaterial(this->scene.models[i].faces[j].materialID);
        }
    }
    for (size_t i=0; i<this->scene.models.size(); i++) {
        for (size_t j=0; j<this->scene.models[i].faces.size(); j++) {
            this->scene.models[i].faces[j].faceMaterial = this->findMaterial(this->scene.models[i].faces[j].materialID);
        }
    }

    return this->scene;
}

void objParser2::fixIndices(
    std::vector<glm::vec3> & in_vertices,
    std::vector<glm::vec2> & in_uvs,
    std::vector<glm::vec3> & in_normals,
    std::vector<unsigned int> & out_indices,
    std::vector<glm::vec3> & out_vertices,
    std::vector<glm::vec2> & out_uvs,
    std::vector<glm::vec3> & out_normals
) {
    std::map<PackedVertex, unsigned int> vertexToOutIndex;
    for (size_t i=0; i<in_vertices.size(); i++) {
        PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };
        unsigned int index;
        bool found = this->getSimilarVertexIndex(packed, vertexToOutIndex, index);
        if (found)
            out_indices.push_back(index);
        else {
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            unsigned int newIndex = (unsigned int)out_vertices.size() - 1;
            out_indices.push_back(newIndex);
            vertexToOutIndex[packed] = newIndex;
        }
    }
}

bool objParser2::getSimilarVertexIndex(PackedVertex & packed, std::map<PackedVertex, unsigned int> & VertexToOutIndex, unsigned int & result) {
    std::map<PackedVertex, unsigned int>::iterator it = VertexToOutIndex.find(packed);
    if (it == VertexToOutIndex.end())
        return false;
    else {
        result = it->second;
        return true;
    }
}

objMaterial objParser2::findMaterial(std::string materialID) {
    for (size_t i=0; i<this->scene.materials.size(); i++) {
        if (materialID == this->scene.materials[i].materialID)
            return this->scene.materials[i];
    }
    return {};
}

std::vector<objMaterial> objParser2::loadMaterial(std::string materialFile) {
    std::vector<objMaterial> materials;

    std::string materialPath = this->file.path.substr(0, this->file.path.find_last_of("\\/")) + "/" + materialFile;
    std::FILE *fp = std::fopen(materialPath.c_str(), "rb");
    if (fp) {
        std::string fileContents;
        std::fseek(fp, 0, SEEK_END);
        fileContents.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&fileContents[0], 1, fileContents.size(), fp);
        std::fclose(fp);

        size_t pos = 0;
        std::string singleLine;

        int indexMaterial = -1;

        while ((pos = fileContents.find(Settings::Instance()->newLineDelimiter)) != std::string::npos) {
            singleLine = fileContents.substr(0, pos);
            std::vector<std::string> lineElements = this->splitString(singleLine, this->regex_whiteSpace);
            lineElements.erase(lineElements.begin());

            std::ostringstream cleanLineOS;
            //copy(lineElements.begin(), lineElements.end(), std::ostream_iterator<std::string>(cleanLineOS, " "));
            copy(lineElements.begin(), lineElements.end(), std::ostream_iterator<std::string>(cleanLineOS, ""));
            std::string cleanLine = cleanLineOS.str();

            if (singleLine == "" || std::regex_match(singleLine, this->regex_comment)) {
                fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
                continue;
            }

            if (std::regex_match(singleLine, this->regex_materialNew)) {
                objMaterial entityMaterial;
                entityMaterial.materialID = std::accumulate(begin(lineElements), end(lineElements), entityMaterial.materialID);
                entityMaterial.specularExp = -1.0;
                entityMaterial.transparency = -1.0;
                entityMaterial.illumination = -1.0;
                entityMaterial.opticalDensity = -1.0;
                indexMaterial += 1;
                materials.push_back(entityMaterial);
            }

            if (std::regex_match(singleLine, this->regex_materialAmbientColor)) {
                std::vector<float> points = this->string2float(lineElements);
                materials[indexMaterial].ambient.r = points[0];
                materials[indexMaterial].ambient.g = points[1];
                materials[indexMaterial].ambient.b = points[2];
                materials[indexMaterial].ambient.a = 1.0;
            }
            else if (std::regex_match(singleLine, this->regex_materialDiffuseColor)) {
                std::vector<float> points = this->string2float(lineElements);
                materials[indexMaterial].diffuse.r = points[0];
                materials[indexMaterial].diffuse.g = points[1];
                materials[indexMaterial].diffuse.b = points[2];
                materials[indexMaterial].diffuse.a = 1.0;
            }
            else if (std::regex_match(singleLine, this->regex_materialSpecularColor)) {
                std::vector<float> points = this->string2float(lineElements);
                materials[indexMaterial].specular.r = points[0];
                materials[indexMaterial].specular.g = points[1];
                materials[indexMaterial].specular.b = points[2];
                materials[indexMaterial].specular.a = 1.0;
            }
            else if (std::regex_match(singleLine, this->regex_materialEmissionColor)) {
                std::vector<float> points = this->string2float(lineElements);
                materials[indexMaterial].emission.r = points[0];
                materials[indexMaterial].emission.g = points[1];
                materials[indexMaterial].emission.b = points[2];
                materials[indexMaterial].emission.a = 1.0;
            }
            else if (std::regex_match(singleLine, this->regex_materialSpecularExp))
                materials[indexMaterial].specularExp = std::stof(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_materialTransperant1) || std::regex_match(singleLine, this->regex_materialTransperant2))
                materials[indexMaterial].transparency = std::stof(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_materialOpticalDensity))
                materials[indexMaterial].opticalDensity = std::stof(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_materialIllumination))
                materials[indexMaterial].illumination = std::stof(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_materialTextureAmbient))
                materials[indexMaterial].textures_ambient = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureDiffuse))
                materials[indexMaterial].textures_diffuse = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureSpecular))
                materials[indexMaterial].textures_specular = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureSpecularExp))
                materials[indexMaterial].textures_specularExp = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureDissolve))
                materials[indexMaterial].textures_dissolve = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureBump))
                materials[indexMaterial].textures_bump = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureDisplacement))
                materials[indexMaterial].textures_displacement = this->parseTextureImage(cleanLine);

            fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
        }
    }

    return materials;
}

#pragma mark - Helpers

objMaterialImage objParser2::parseTextureImage(std::string textureLine) {
    objMaterialImage materialImage;

    materialImage.height = 0;
    materialImage.width = 0;
    materialImage.useTexture = true;

    if (textureLine.find("-") != std::string::npos) {
        std::regex dash("-");
        std::vector<std::string> lineElements = this->splitString(textureLine, dash);

        if (lineElements[0] == "")
            lineElements.erase(lineElements.begin());

        std::vector<std::string> lastElements = this->splitString(lineElements[lineElements.size() - 1], this->regex_whiteSpace);
        materialImage.image = lastElements[lastElements.size() - 1];
        lastElements.erase(lastElements.end() - 1);

        std::ostringstream lastCommandsOS;
        copy(lastElements.begin(), lastElements.end(), std::ostream_iterator<std::string>(lastCommandsOS, " "));
        std::string lastCommand = lastCommandsOS.str();
        lineElements[lineElements.size() - 1] = lastCommand;

        for (size_t i=0; i<lineElements.size(); i++) {
            materialImage.commands.push_back("-" + lineElements[i]);
        }
    }
    else
        materialImage.image = textureLine;

    std::string folderPath = this->file.path;
    boost::replace_all(folderPath, this->file.title, "");
#ifdef _WIN32
    if (!boost::filesystem::exists(materialImage.image))
        materialImage.image = folderPath + "/" + materialImage.image;
#else
    if (!boost::filesystem::exists(materialImage.image))
        materialImage.image = folderPath + "/" + materialImage.image;
#endif

    std::regex pathSeparator("/");
    std::vector<std::string> fileElements = this->splitString(materialImage.image, pathSeparator);
    materialImage.filename = fileElements[fileElements.size() - 1];

    return materialImage;
}

std::vector<std::string> objParser2::splitString(const std::string &s, std::regex delimiter) {
    std::vector<std::string> elements;
    std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        elements.push_back(*iter);
    return elements;
}

std::vector<float> objParser2::string2float(std::vector<std::string> strings) {
    std::vector<float> floats;
    try {
        for (size_t i=0; i<strings.size(); i++) {
            floats.push_back(std::stof(strings[i]));
        }
    }
    catch (const std::exception &) { }
    return floats;
}

int objParser2::getLineCount() {
    char delim;
#ifdef _WIN32
    delim = '\r\n';
#elif defined macintosh // OS 9
    delim = '\r';
#else
    delim = '\n';
#endif

    std::ifstream inFile2(this->file.path.c_str());
    int linesCount = (int)std::count(std::istreambuf_iterator<char>(inFile2), std::istreambuf_iterator<char>(), delim);
    inFile2.close();
    return linesCount;
}
