//
//  ModelFace.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFace.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "kuplung/utilities/stb/stb_image.h"

ModelFace::ModelFace() {
}

ModelFace* ModelFace::clone(int modelID) {
    ModelFace *mmf = new ModelFace();

    mmf->ModelID = modelID;

    mmf->matrixCamera = this->matrixCamera;
    mmf->matrixModel = this->matrixModel;
    mmf->matrixProjection = this->matrixProjection;

    mmf->Setting_UseTessellation = this->Setting_UseTessellation;
    mmf->ModelID = this->ModelID;
    mmf->meshModel = this->meshModel;

    mmf->so_fov = this->so_fov;
    mmf->so_outlineThickness = this->so_outlineThickness;
    mmf->so_outlineColor = this->so_outlineColor;

    mmf->vecCameraPosition = this->vecCameraPosition;

    mmf->GLSL_LightSourceNumber_Directional = this->GLSL_LightSourceNumber_Directional;
    mmf->GLSL_LightSourceNumber_Point = this->GLSL_LightSourceNumber_Point;
    mmf->GLSL_LightSourceNumber_Spot = this->GLSL_LightSourceNumber_Spot;
    mmf->mfLights_Directional = this->mfLights_Directional;
    mmf->mfLights_Point = this->mfLights_Point;
    mmf->mfLights_Spot = this->mfLights_Spot;

    mmf->init();
    mmf->setModel(mmf->meshModel);

    return mmf;
}

#pragma mark - Destroy

ModelFace::~ModelFace() {
    this->destroy();
}

void ModelFace::destroy() {
    for (size_t i=0; i<this->mfLights_Directional.size(); i++) {
        delete this->mfLights_Directional[i];
    }
    for (size_t i=0; i<this->mfLights_Point.size(); i++) {
        delete this->mfLights_Point[i];
    }
    for (size_t i=0; i<this->mfLights_Spot.size(); i++) {
        delete this->mfLights_Spot[i];
    }

    this->meshModel = {};

    delete this->positionX;
    delete this->positionY;
    delete this->positionZ;
    delete this->scaleX;
    delete this->scaleY;
    delete this->scaleZ;
    delete this->rotateX;
    delete this->rotateY;
    delete this->rotateZ;
    delete this->displaceX;
    delete this->displaceY;
    delete this->displaceZ;
    delete this->Setting_MaterialRefraction;
    delete this->Setting_MaterialSpecularExp;
    delete this->displacementHeightScale;
    delete this->materialAmbient;
    delete this->materialDiffuse;
    delete this->materialSpecular;
    delete this->materialEmission;
    delete this->Effect_GBlur_Radius;
    delete this->Effect_GBlur_Width;
}

#pragma mark - Initialization

void ModelFace::init() {
    this->glUtils = new GLUtils();
    this->mathHelper = new Maths();

    this->initBuffersAgain = false;

    this->so_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->Setting_UseCullFace = false;
    this->Setting_UseTessellation = true;
    this->Setting_TessellationSubdivision = 1;
    this->showMaterialEditor = false;
    this->GLSL_LightSourceNumber_Directional = 8;
    this->GLSL_LightSourceNumber_Point = 4;
    this->GLSL_LightSourceNumber_Spot = 4;
    this->Setting_LightingPass_DrawMode = 1;

    // light
    this->Setting_LightStrengthAmbient = 0.5;
    this->Setting_LightStrengthDiffuse = 1.0;
    this->Setting_LightStrengthSpecular = 0.5;
    this->Setting_LightAmbient = glm::vec3(1.0, 1.0, 1.0);
    this->Setting_LightDiffuse = glm::vec3(1.0, 1.0, 1.0);
    this->Setting_LightSpecular = glm::vec3(1.0, 1.0, 1.0);

    // material
    this->Setting_MaterialRefraction = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ this->meshModel.ModelMaterial.OpticalDensity });
    this->materialAmbient = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialDiffuse = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialSpecular = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialEmission = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->displacementHeightScale = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->Setting_ParallaxMapping = false;

    // effects
    this->Effect_GBlur_Mode = -1;
    this->Effect_GBlur_Radius = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->Effect_GBlur_Width = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
}

void ModelFace::setModel(MeshModel meshModel) {
    this->meshModel = meshModel;
}

void ModelFace::initModelProperties() {
    this->Setting_CelShading = false;
    this->Setting_Wireframe = false;
    this->Setting_Alpha = 1.0f;
    this->showMaterialEditor = false;

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->displaceX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->displaceY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->displaceZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->matrixModel = glm::mat4(1.0);

    this->Setting_MaterialRefraction = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ this->meshModel.ModelMaterial.OpticalDensity });
    this->Setting_MaterialSpecularExp = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ this->meshModel.ModelMaterial.SpecularExp });

    this->Setting_LightPosition = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDirection = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightAmbient = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDiffuse = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightSpecular = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightStrengthAmbient = 1.0;
    this->Setting_LightStrengthDiffuse = 1.0;
    this->Setting_LightStrengthSpecular = 1.0;
    this->Setting_TessellationSubdivision = 1;
    this->Setting_LightingPass_DrawMode = 1;

    this->materialIlluminationModel = this->meshModel.ModelMaterial.IlluminationMode;
    this->Setting_ParallaxMapping = false;

    this->materialAmbient = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ this->meshModel.ModelMaterial.AmbientColor });
    this->materialDiffuse = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ this->meshModel.ModelMaterial.DiffuseColor });
    this->materialSpecular = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ this->meshModel.ModelMaterial.SpecularColor });
    this->materialEmission = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ this->meshModel.ModelMaterial.EmissionColor });
    this->displacementHeightScale = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->Effect_GBlur_Mode = -1;
    this->Effect_GBlur_Radius = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->Effect_GBlur_Width = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->Effect_Bloom_doBloom = false;
    this->Effect_Bloom_WeightA = 0.0f;
    this->Effect_Bloom_WeightB = 0.0f;
    this->Effect_Bloom_WeightC = 0.0f;
    this->Effect_Bloom_WeightD = 0.0f;
    this->Effect_Bloom_Vignette = 0.0f;
    this->Effect_Bloom_VignetteAtt = 0.0f;
}

void ModelFace::initProperties() {
    this->Setting_CelShading = false;
    this->Setting_Alpha = 1.0f;

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->displaceX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->displaceY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->displaceZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->matrixModel = glm::mat4(1.0);

    this->Setting_MaterialRefraction = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->Setting_MaterialSpecularExp = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 100.0f });

    this->Setting_LightPosition = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDirection = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightAmbient = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDiffuse = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightSpecular = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightStrengthAmbient = 1.0;
    this->Setting_LightStrengthDiffuse = 1.0;
    this->Setting_LightStrengthSpecular = 1.0;
    this->Setting_TessellationSubdivision = 1;
    this->Setting_LightingPass_DrawMode = 1;

    this->materialIlluminationModel = 1;
    this->Setting_ParallaxMapping = false;
    this->materialAmbient = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialDiffuse = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialSpecular = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialEmission = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });

    this->Effect_GBlur_Mode = -1;
    this->Effect_GBlur_Radius = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->Effect_GBlur_Width = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->Effect_Bloom_doBloom = false;
    this->Effect_Bloom_WeightA = 0.0f;
    this->Effect_Bloom_WeightB = 0.0f;
    this->Effect_Bloom_WeightC = 0.0f;
    this->Effect_Bloom_WeightD = 0.0f;
    this->Effect_Bloom_Vignette = 0.0f;
    this->Effect_Bloom_VignetteAtt = 0.0f;
}

bool ModelFace::initShaderProgram() {
    return true;
}

void ModelFace::initBuffers(std::string assetsFolder) {
}

void ModelFace::initBoundingBox() {
    this->boundingBox = new BoundingBox();
    this->boundingBox->initShaderProgram();
    this->boundingBox->initBuffers(this->meshModel);
}

void ModelFace::loadTexture(std::string assetsFolder, MeshMaterialTextureImage materialImage, objMaterialImageType type, GLuint* vboObject) {
    if (materialImage.Image != "") {
        std::string matImageLocal = materialImage.Image;
        if (!boost::filesystem::exists(matImageLocal))
            matImageLocal = assetsFolder + "/" + materialImage.Image;

        int tWidth, tHeight, tChannels;
        unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
        if (!tPixels) {
            std::string texName = "";
            switch (type) {
                case objMaterialImageType_Ambient:
                    texName = "ambient";
                    break;
                case objMaterialImageType_Diffuse:
                    texName = "diffuse";
                    break;
                case objMaterialImageType_Specular:
                    texName = "specular";
                    break;
                case objMaterialImageType_SpecularExp:
                    texName = "specularExp";
                    break;
                case objMaterialImageType_Dissolve:
                    texName = "dissolve";
                    break;
                case objMaterialImageType_Bump:
                    texName = "bump";
                    break;
                case objMaterialImageType_Displacement:
                    texName = "displacement";
                    break;
                default:
                    break;
            }
            Settings::Instance()->funcDoLog("Can't load " + texName + " texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
        }
        else {
            glGenTextures(1, vboObject);
            glBindTexture(GL_TEXTURE_2D, *vboObject);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);

            GLint textureFormat = 0;
            switch (tChannels) {
                case 1:
                    textureFormat = GL_LUMINANCE;
                    break;
                case 2:
                    textureFormat = GL_LUMINANCE_ALPHA;
                    break;
                case 3:
                    textureFormat = GL_RGB;
                    break;
                case 4:
                    textureFormat = GL_RGBA;
                    break;
                default:
                    textureFormat = GL_RGB;
                    break;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, tWidth, tHeight, 0, textureFormat, GL_UNSIGNED_BYTE, tPixels);
            stbi_image_free(tPixels);
        }
    }
}

void ModelFace::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
    this->matrixModel = matrixModel;
    this->vecCameraPosition = vecCameraPosition;
    this->grid = grid;
    this->uiAmbientLight = uiAmbientLight;
}

#pragma mark - Scene Options

void ModelFace::setOptionsFOV(float fov) {
    this->so_fov = fov;
}

void ModelFace::setOptionsSelected(bool selectedYn) {
    this->so_selectedYn = selectedYn;
}

void ModelFace::setOptionsOutlineColor(glm::vec4 outlineColor) {
    this->so_outlineColor = outlineColor;
}

void ModelFace::setOptionsOutlineThickness(float thickness) {
    this->so_outlineThickness = thickness;
}

#pragma mark - Utilities

std::string ModelFace::readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        Settings::Instance()->funcDoLog("Could not read file " + std::string(filePath) + ". File does not exist.");
        return "";
    }
    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}
