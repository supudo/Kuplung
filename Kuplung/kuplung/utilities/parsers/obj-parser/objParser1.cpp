//
//  objParser1.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/19/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "objParser1.hpp"
#include <fstream>
#include <numeric>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

objParser1::~objParser1() {
}

void objParser1::init(std::function<void(float)> doProgress) {
    this->models = {};
    this->materials = {};
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

    this->parserUtils = std::make_unique<ParserUtils>();
}

std::vector<MeshModel> objParser1::parse(const FBEntity& fileToParse, const std::vector<std::string>&) {
    this->file = fileToParse;
    this->geometricVertices = {};
    this->textureCoordinates = {};
    this->vertexNormals = {};
    this->spaceVertices = {};
    this->polygonalFaces = {};
    this->models = {};
    this->materials = {};

    std::FILE *fp = std::fopen(this->file.path.c_str(), "rb");
    if (fp) {
        std::string fileContents;
        std::fseek(fp, 0, SEEK_END);
        fileContents.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&fileContents[0], 1, fileContents.size(), fp);
        std::fclose(fp);

        size_t pos = 0;
        std::string singleLine;

        int indexModel = -1;
        int indicesCounter = 0;

        this->objFileLinesCount = this->getLineCount();
        int linesProcessedCounter = 0;

        int modelID = 1;
        while ((pos = fileContents.find(Settings::Instance()->newLineDelimiter)) != std::string::npos) {
            singleLine = fileContents.substr(0, pos);

            if (singleLine.empty() || std::regex_match(singleLine, this->regex_comment)) {
                fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
                continue;
            }

            std::vector<std::string> lineElements = this->splitString(singleLine, this->regex_whiteSpace);
            lineElements.erase(lineElements.begin());

            if (std::regex_match(singleLine, this->regex_objTitle) || std::regex_match(singleLine, this->regex_materialFile)) {
                if (std::regex_match(singleLine, this->regex_objTitle)) {
                    MeshModel entityModel;
                    entityModel.ID = modelID;
                    entityModel.File = file;
                    entityModel.ModelTitle = std::accumulate(begin(lineElements), end(lineElements), entityModel.ModelTitle);
                    entityModel.countVertices = 0;
                    entityModel.countTextureCoordinates = 0;
                    entityModel.countNormals = 0;
                    entityModel.countIndices = 0;
                    indexModel += 1;
                    modelID += 1;
                    this->models.push_back(entityModel);
                }
            }

            if (std::regex_match(singleLine, this->regex_materialFile))
                this->materials = this->loadMaterial(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_geometricVertices)) {
                std::vector<float> points = this->string2float(lineElements);
                this->geometricVertices.insert(end(this->geometricVertices), begin(points), end(points));
                //this->vectorsVertices.push_back(glm::vec3(points[0], points[1], points[2]));
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
                this->models[static_cast<size_t>(indexModel)].MaterialTitle = singleLine;
                boost::replace_all(this->models[static_cast<size_t>(indexModel)].MaterialTitle, "usemtl ", "");
            }
            else if (std::regex_match(singleLine, this->regex_polygonalFaces)) {
                std::vector<std::string> singleFaceElements = this->splitString(singleLine, this->regex_whiteSpace);
                singleFaceElements.erase(singleFaceElements.begin());

                for (size_t i=0; i<singleFaceElements.size(); i++) {
                    std::vector<std::string> face = this->splitString(singleFaceElements[i], this->regex_polygonalFacesSingle);

                    int v_idx = (std::stoi(face[0]) - 1) * 3;
                    glm::vec3 v_v = glm::vec3(this->geometricVertices[static_cast<size_t>(v_idx + 0)],
                            this->geometricVertices[static_cast<size_t>(v_idx + 1)],
                            this->geometricVertices[static_cast<size_t>(v_idx + 2)]);
                    this->models[static_cast<size_t>(indexModel)].vertices.push_back(v_v);
                    this->models[static_cast<size_t>(indexModel)].countVertices += 3;

                    if (this->textureCoordinates.size() > 0 && !face[1].empty()) {
                        int t_idx = (std::stoi(face[1]) - 1) * 2;
                        glm::vec2 v_tc = glm::vec2(this->textureCoordinates[static_cast<size_t>(t_idx + 0)],
                                this->textureCoordinates[static_cast<size_t>(t_idx + 1)]);
                        this->models[static_cast<size_t>(indexModel)].texture_coordinates.push_back(v_tc);
                        this->models[static_cast<size_t>(indexModel)].countTextureCoordinates += 2;
                    }

                    int n_idx = (std::stoi(face[2]) - 1) * 3;
                    glm::vec3 v_n = glm::vec3(this->vertexNormals[static_cast<size_t>(n_idx + 0)],
                            this->vertexNormals[static_cast<size_t>(n_idx + 1)],
                            this->vertexNormals[static_cast<size_t>(n_idx + 2)]);
                    this->models[static_cast<size_t>(indexModel)].normals.push_back(v_n);
                    this->models[static_cast<size_t>(indexModel)].countNormals += 3;

                    this->models[static_cast<size_t>(indexModel)].indices.push_back(static_cast<unsigned int>(indicesCounter));
                    this->models[static_cast<size_t>(indexModel)].countIndices += 1;
                    indicesCounter += 1;
                }
            }

            linesProcessedCounter += 1;
            float progress = (float(linesProcessedCounter) / float(this->objFileLinesCount)) * 100.0f;
            this->doProgress(progress);
            fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
        }
    }
    for (size_t i=0; i<this->models.size(); i++) {
        this->models[i].ModelMaterial = this->findMaterial(this->models[i].MaterialTitle);
    }

    //Kuplung_printObjModels(this->models, false);

    return this->models;
}

MeshModelMaterial objParser1::findMaterial(std::string const& materialID) {
    for (size_t i=0; i<this->materials.size(); i++) {
        if (materialID == this->materials[i].MaterialTitle)
            return this->materials[i];
    }
    return {};
}

std::vector<MeshModelMaterial> objParser1::loadMaterial(std::string const& materialFile) {
    std::vector<MeshModelMaterial> current_materials;

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

            if (singleLine.empty() || std::regex_match(singleLine, this->regex_comment)) {
                fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
                continue;
            }

            if (std::regex_match(singleLine, this->regex_materialNew)) {
                MeshModelMaterial entityMaterial = {};
                entityMaterial.MaterialTitle = std::accumulate(begin(lineElements), end(lineElements), entityMaterial.MaterialTitle);
                entityMaterial.SpecularExp = 1.0;
                entityMaterial.Transparency = 1.0;
                entityMaterial.IlluminationMode = 2;
                entityMaterial.OpticalDensity = 1.0;
                indexMaterial += 1;
				current_materials.push_back(entityMaterial);
            }

            if (std::regex_match(singleLine, this->regex_materialAmbientColor)) {
                std::vector<float> points = this->string2float(lineElements);
				current_materials[static_cast<size_t>(indexMaterial)].AmbientColor.r = points[0];
				current_materials[static_cast<size_t>(indexMaterial)].AmbientColor.g = points[1];
				current_materials[static_cast<size_t>(indexMaterial)].AmbientColor.b = points[2];
            }
            else if (std::regex_match(singleLine, this->regex_materialDiffuseColor)) {
                std::vector<float> points = this->string2float(lineElements);
				current_materials[static_cast<size_t>(indexMaterial)].DiffuseColor.r = points[0];
				current_materials[static_cast<size_t>(indexMaterial)].DiffuseColor.g = points[1];
				current_materials[static_cast<size_t>(indexMaterial)].DiffuseColor.b = points[2];
            }
            else if (std::regex_match(singleLine, this->regex_materialSpecularColor)) {
                std::vector<float> points = this->string2float(lineElements);
				current_materials[static_cast<size_t>(indexMaterial)].SpecularColor.r = points[0];
				current_materials[static_cast<size_t>(indexMaterial)].SpecularColor.g = points[1];
				current_materials[static_cast<size_t>(indexMaterial)].SpecularColor.b = points[2];
            }
            else if (std::regex_match(singleLine, this->regex_materialEmissionColor)) {
                std::vector<float> points = this->string2float(lineElements);
				current_materials[static_cast<size_t>(indexMaterial)].EmissionColor.r = points[0];
				current_materials[static_cast<size_t>(indexMaterial)].EmissionColor.g = points[1];
				current_materials[static_cast<size_t>(indexMaterial)].EmissionColor.b = points[2];
            }
            else if (std::regex_match(singleLine, this->regex_materialSpecularExp))
				current_materials[static_cast<size_t>(indexMaterial)].SpecularExp = std::stof(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_materialTransperant1) || std::regex_match(singleLine, this->regex_materialTransperant2))
				current_materials[static_cast<size_t>(indexMaterial)].Transparency = std::stof(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_materialOpticalDensity))
				current_materials[static_cast<size_t>(indexMaterial)].OpticalDensity = std::stof(lineElements[0]);
            else if (std::regex_match(singleLine, this->regex_materialIllumination))
				current_materials[static_cast<size_t>(indexMaterial)].IlluminationMode = static_cast<unsigned int>(std::stof(lineElements[0]));
            else if (std::regex_match(singleLine, this->regex_materialTextureAmbient))
				current_materials[static_cast<size_t>(indexMaterial)].TextureAmbient = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureDiffuse))
				current_materials[static_cast<size_t>(indexMaterial)].TextureDiffuse = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureSpecular))
				current_materials[static_cast<size_t>(indexMaterial)].TextureSpecular = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureSpecularExp))
				current_materials[static_cast<size_t>(indexMaterial)].TextureSpecularExp = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureDissolve))
				current_materials[static_cast<size_t>(indexMaterial)].TextureDissolve = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureBump))
				current_materials[static_cast<size_t>(indexMaterial)].TextureBump = this->parseTextureImage(cleanLine);
            else if (std::regex_match(singleLine, this->regex_materialTextureDisplacement))
				current_materials[static_cast<size_t>(indexMaterial)].TextureDisplacement = this->parseTextureImage(cleanLine);

            fileContents.erase(0, pos + Settings::Instance()->newLineDelimiter.length());
        }
    }

    return current_materials;
}

#pragma mark - Helpers

MeshMaterialTextureImage objParser1::parseTextureImage(std::string const& textureLine) {
    MeshMaterialTextureImage materialImage;

    materialImage.Height = 0;
    materialImage.Width = 0;
    materialImage.UseTexture = true;

    if (textureLine.find('-') != std::string::npos) {
        std::regex dash("-");
        std::vector<std::string> lineElements = this->splitString(textureLine, dash);

        if (lineElements[0].empty())
            lineElements.erase(lineElements.begin());

        std::vector<std::string> lastElements = this->splitString(lineElements[lineElements.size() - 1], this->regex_whiteSpace);
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

    std::string folderPath = this->file.path;
    boost::replace_all(folderPath, this->file.title, "");
    if (!boost::filesystem::exists(materialImage.Image) && !boost::filesystem::path(materialImage.Image).is_absolute())
        materialImage.Image = folderPath + materialImage.Image;

    std::regex pathSeparator("/");
    std::vector<std::string> fileElements = this->splitString(materialImage.Image, pathSeparator);
    materialImage.Filename = fileElements[fileElements.size() - 1];

    return materialImage;
}

std::vector<std::string> objParser1::splitString(const std::string &s, const std::regex& delimiter) {
    std::vector<std::string> elements;
    std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        elements.push_back(*iter);
    return elements;
}

std::vector<float> objParser1::string2float(const std::vector<std::string>& strings) {
    std::vector<float> floats;
    try {
        for (size_t i=0; i<strings.size(); i++) {
            floats.push_back(std::stof(strings[i]));
        }
    }
    catch (const std::exception &) { }
    return floats;
}

int objParser1::getLineCount() {
#ifdef _WIN32
    return 0;
#else
    char delim;
#if defined macintosh // OS 9
    delim = '\r';
#else
    delim = '\n';
#endif

    std::ifstream inFile2(this->file.path.c_str());
    int linesCount = int(std::count(std::istreambuf_iterator<char>(inFile2), std::istreambuf_iterator<char>(), delim));
    inFile2.close();
    return linesCount;
#endif
}
