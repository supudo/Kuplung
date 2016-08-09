//
//  ModelFaceBase.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceBase.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "kuplung/utilities/stb/stb_image.h"

ModelFaceBase::ModelFaceBase() {
//    this->positionX = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->positionY = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->positionZ = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->scaleX = std::make_unique<ObjectCoordinate>(false, 1.0f });
//    this->scaleY = std::make_unique<ObjectCoordinate>(false, 1.0f });
//    this->scaleZ = std::make_unique<ObjectCoordinate>(false, 1.0f });
//    this->rotateX = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->rotateY = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->displaceX = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->displaceY = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->displaceZ = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->materialAmbient = new MaterialColor(alse, false, 1.0, glm::vec3(1.0, 1.0, 1.0) });
//    this->materialDiffuse = new MaterialColor(alse, false, 1.0, glm::vec3(1.0, 1.0, 1.0) });
//    this->materialSpecular = new MaterialColor(alse, false, 1.0, glm::vec3(1.0, 1.0, 1.0) });
//    this->materialEmission = new MaterialColor(alse, false, 1.0, glm::vec3(1.0, 1.0, 1.0) });
//    this->Setting_MaterialSpecularExp = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->Setting_MaterialRefraction = std::make_unique<ObjectCoordinate>(false, 1.0f });
//    this->displacementHeightScale = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->Effect_GBlur_Radius = std::make_unique<ObjectCoordinate>(false, 0.0f });
//    this->Effect_GBlur_Width = std::make_unique<ObjectCoordinate>(false, 0.0f });
}

ModelFaceBase* ModelFaceBase::clone(int modelID) {
    ModelFaceBase *mmf = new ModelFaceBase();

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

    mmf->init(mmf->meshModel, mmf->assetsFolder);

    return mmf;
}

#pragma mark - Destroy

ModelFaceBase::~ModelFaceBase() {
    this->destroy();
}

void ModelFaceBase::destroy() {
    this->meshModel = {};

    this->positionX.reset();
    this->positionY.reset();
    this->positionZ.reset();
    this->scaleX.reset();
    this->scaleY.reset();
    this->scaleZ.reset();
    this->rotateX.reset();
    this->rotateY.reset();
    this->rotateZ.reset();
    this->displaceX.reset();
    this->displaceY.reset();
    this->displaceZ.reset();
    this->Setting_MaterialRefraction.reset();
    this->Setting_MaterialSpecularExp.reset();
    this->displacementHeightScale.reset();
    this->materialAmbient.reset();
    this->materialDiffuse.reset();
    this->materialSpecular.reset();
    this->materialEmission.reset();
    this->Effect_GBlur_Radius.reset();
    this->Effect_GBlur_Width.reset();
}

#pragma mark - Initialization

void ModelFaceBase::init(MeshModel model, std::string assetsFolder) {
    this->glUtils = new GLUtils();
    this->mathHelper = new Maths();

    this->meshModel = model;
    this->assetsFolder = assetsFolder;

    this->initBuffersAgain = false;
    this->Settings_DeferredRender = false;

    this->so_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->Setting_UseCullFace = false;
    this->Setting_UseTessellation = true;
    this->Setting_TessellationSubdivision = 1;
    this->showMaterialEditor = false;
    this->Setting_LightingPass_DrawMode = 2;

    // light
    this->Setting_LightStrengthAmbient = 0.5;
    this->Setting_LightStrengthDiffuse = 1.0;
    this->Setting_LightStrengthSpecular = 0.5;
    this->Setting_LightAmbient = glm::vec3(1.0, 1.0, 1.0);
    this->Setting_LightDiffuse = glm::vec3(1.0, 1.0, 1.0);
    this->Setting_LightSpecular = glm::vec3(1.0, 1.0, 1.0);

    // material
    this->Setting_MaterialRefraction = std::make_unique<ObjectCoordinate>(false, this->meshModel.ModelMaterial.OpticalDensity);
    this->materialAmbient = std::make_unique<MaterialColor>(false, false, 1.0, glm::vec3(1.0, 1.0, 1.0));
    this->materialDiffuse = std::make_unique<MaterialColor>(false, false, 1.0, glm::vec3(1.0, 1.0, 1.0));
    this->materialSpecular = std::make_unique<MaterialColor>(false, false, 1.0, glm::vec3(1.0, 1.0, 1.0));
    this->materialEmission = std::make_unique<MaterialColor>(false, false, 1.0, glm::vec3(1.0, 1.0, 1.0));
    this->displacementHeightScale = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->Setting_ParallaxMapping = false;

    // effects
    this->Effect_GBlur_Mode = -1;
    this->Effect_GBlur_Radius = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->Effect_GBlur_Width = std::make_unique<ObjectCoordinate>(false, 0.0f);
}

void ModelFaceBase::initModelProperties() {
    this->Setting_CelShading = false;
    this->Setting_Wireframe = false;
    this->Setting_Alpha = 1.0f;
    this->showMaterialEditor = false;
    this->Settings_DeferredRender = false;

    this->positionX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->positionY = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->positionZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->scaleX = std::make_unique<ObjectCoordinate>(false, 1.0f);
    this->scaleY = std::make_unique<ObjectCoordinate>(false, 1.0f);
    this->scaleZ = std::make_unique<ObjectCoordinate>(false, 1.0f);

    this->rotateX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateY = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->displaceX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->displaceY = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->displaceZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->matrixModel = glm::mat4(1.0);

    this->Setting_MaterialRefraction = std::make_unique<ObjectCoordinate>(false, this->meshModel.ModelMaterial.OpticalDensity);
    this->Setting_MaterialSpecularExp = std::make_unique<ObjectCoordinate>(false, this->meshModel.ModelMaterial.SpecularExp);

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

    this->materialAmbient = std::make_unique<MaterialColor>(false, false, 1.0, this->meshModel.ModelMaterial.AmbientColor);
    this->materialDiffuse = std::make_unique<MaterialColor>(false, false, 1.0, this->meshModel.ModelMaterial.DiffuseColor);
    this->materialSpecular = std::make_unique<MaterialColor>(false, false, 1.0, this->meshModel.ModelMaterial.SpecularColor);
    this->materialEmission = std::make_unique<MaterialColor>(false, false, 1.0, this->meshModel.ModelMaterial.EmissionColor);
    this->displacementHeightScale = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->Effect_GBlur_Mode = -1;
    this->Effect_GBlur_Radius = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->Effect_GBlur_Width = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->Effect_Bloom_doBloom = false;
    this->Effect_Bloom_WeightA = 0.0f;
    this->Effect_Bloom_WeightB = 0.0f;
    this->Effect_Bloom_WeightC = 0.0f;
    this->Effect_Bloom_WeightD = 0.0f;
    this->Effect_Bloom_Vignette = 0.0f;
    this->Effect_Bloom_VignetteAtt = 0.0f;
}

void ModelFaceBase::initBuffers() {
}

void ModelFaceBase::initBoundingBox() {
    this->boundingBox = new BoundingBox();
    this->boundingBox->initShaderProgram();
    this->boundingBox->initBuffers(this->meshModel);
}

void ModelFaceBase::loadTexture(std::string assetsFolder, MeshMaterialTextureImage materialImage, objMaterialImageType type, GLuint* vboObject) {
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

void ModelFaceBase::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
    this->matrixModel = matrixModel;
    this->vecCameraPosition = vecCameraPosition;
    this->grid = grid;
    this->uiAmbientLight = uiAmbientLight;
}

#pragma mark - Scene Options

void ModelFaceBase::setOptionsFOV(float fov) {
    this->so_fov = fov;
}

void ModelFaceBase::setOptionsSelected(bool selectedYn) {
    this->so_selectedYn = selectedYn;
}

void ModelFaceBase::setOptionsOutlineColor(glm::vec4 outlineColor) {
    this->so_outlineColor = outlineColor;
}

void ModelFaceBase::setOptionsOutlineThickness(float thickness) {
    this->so_outlineThickness = thickness;
}

bool ModelFaceBase::getOptionsSelected() {
    return this->so_selectedYn;
}

glm::vec4 ModelFaceBase::getOptionsOutlineColor() {
    return this->so_outlineColor;
}

float ModelFaceBase::getOptionsOutlineThickness() {
    return this->so_outlineThickness;
}
