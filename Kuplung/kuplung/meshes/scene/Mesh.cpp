//
//  Mesh.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Mesh.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

Mesh::Mesh() {
}

Mesh* Mesh::clone(int modelID) {
    Mesh *mmf = new Mesh();

    mmf->ModelID = modelID;

    mmf->Setting_UseTessellation = this->Setting_UseTessellation;
    mmf->ModelID = this->ModelID;
    mmf->meshModel = this->meshModel;

    mmf->init();
    mmf->setModel(mmf->meshModel);
    mmf->initBuffers(Settings::Instance()->currentFolder);

    return mmf;
}

#pragma mark - Destroy

Mesh::~Mesh() {
    this->destroy();
}

void Mesh::destroy() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboNormals);
    glDeleteBuffers(1, &this->vboTextureCoordinates);
    glDeleteBuffers(1, &this->vboIndices);
    glDeleteBuffers(1, &this->vboTangents);
    glDeleteBuffers(1, &this->vboBitangents);

    GLint maxColorAttachments = 1;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    GLint colorAttachment;
    GLenum att = GL_COLOR_ATTACHMENT0;
    for (colorAttachment = 0; colorAttachment < maxColorAttachments; colorAttachment++) {
        att += colorAttachment;
        GLint param;
        GLuint objName;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
        if (GL_RENDERBUFFER == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteRenderbuffers(1, &objName);
        }
        else if (GL_TEXTURE == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteTextures(1, &objName);
        }
    }

    glDeleteVertexArrays(1, &this->glVAO);

    this->boundingBox->destroy();

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

void Mesh::init() {
    this->glUtils = new GLUtils();
    this->mathHelper = new Maths();

    this->initBuffersAgain = false;

    this->so_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->Setting_UseCullFace = false;
    this->Setting_UseTessellation = true;
    this->Setting_TessellationSubdivision = 1;
    this->showMaterialEditor = false;
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

void Mesh::setModel(MeshModel meshModel) {
    this->meshModel = meshModel;
}

void Mesh::initModelProperties() {
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

void Mesh::initProperties() {
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

#pragma mark - Public

void Mesh::initBuffers(std::string assetsFolder) {
    this->assetsFolder = assetsFolder;

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.vertices.size() * sizeof(glm::vec3), &this->meshModel.vertices[0], GL_STATIC_DRAW);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.normals.size() * sizeof(glm::vec3), &this->meshModel.normals[0], GL_STATIC_DRAW);

    // textures and colors
    if (this->meshModel.texture_coordinates.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->meshModel.texture_coordinates.size() * sizeof(glm::vec2), &this->meshModel.texture_coordinates[0], GL_STATIC_DRAW);

        // ambient texture image
        this->loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureAmbient, objMaterialImageType_Bump, &this->vboTextureAmbient);

        // diffuse texture image
        this->loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDiffuse, objMaterialImageType_Bump, &this->vboTextureDiffuse);

        // specular texture image
        this->loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecular, objMaterialImageType_Specular, &this->vboTextureSpecular);

        // specular-exp texture image
        this->loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecularExp, objMaterialImageType_SpecularExp, &this->vboTextureSpecularExp);

        // dissolve texture image
        this->loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDissolve, objMaterialImageType_Dissolve, &this->vboTextureDissolve);

        // bump map texture
        this->loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureBump, objMaterialImageType_Bump, &this->vboTextureBump);

        // displacement map texture
        this->loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDisplacement, objMaterialImageType_Displacement, &this->vboTextureDisplacement);
    }

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->meshModel.countIndices * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

    if (this->meshModel.ModelMaterial.TextureBump.Image != "" &&
        this->meshModel.vertices.size() > 0 &&
        this->meshModel.texture_coordinates.size() > 0 &&
        this->meshModel.normals.size() > 0) {
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        this->mathHelper->computeTangentBasis(this->meshModel.vertices, this->meshModel.texture_coordinates, this->meshModel.normals, tangents, bitangents);

        // tangents
        glGenBuffers(1, &this->vboTangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTangents);
        glBufferData(GL_ARRAY_BUFFER, (int)tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

        // bitangents
        glGenBuffers(1, &this->vboBitangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboBitangents);
        glBufferData(GL_ARRAY_BUFFER, (int)bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);

    this->initBuffersAgain = false;
}

void Mesh::initBoundingBox() {
    this->boundingBox = new BoundingBox();
    this->boundingBox->initShaderProgram();
    this->boundingBox->initBuffers(this->meshModel);
}

void Mesh::loadTexture(std::string assetsFolder, MeshMaterialTextureImage materialImage, objMaterialImageType type, GLuint* vboObject) {
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

#pragma mark - Render

void Mesh::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
    if (this->initBuffersAgain)
        this->initBuffers(this->assetsFolder);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBindVertexArray(this->glVAO);

    if (this->glVAO > 0) {
        if (this->vboTextureAmbient > 0 && this->meshModel.ModelMaterial.TextureAmbient.UseTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureAmbient);
        }

        if (this->vboTextureDiffuse > 0 && this->meshModel.ModelMaterial.TextureDiffuse.UseTexture) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
        }

        if (this->vboTextureSpecular > 0 && this->meshModel.ModelMaterial.TextureSpecular.UseTexture) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecular);
        }

        if (this->vboTextureSpecularExp > 0 && this->meshModel.ModelMaterial.TextureSpecularExp.UseTexture) {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecularExp);
        }

        if (this->vboTextureDissolve > 0 && this->meshModel.ModelMaterial.TextureDissolve.UseTexture) {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDissolve);
        }

        if (this->vboTextureBump > 0 && this->meshModel.ModelMaterial.TextureBump.UseTexture) {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureBump);
        }

        if (this->vboTextureDisplacement > 0 && this->meshModel.ModelMaterial.TextureDisplacement.UseTexture) {
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDisplacement);
        }

        if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawElements(GL_PATCHES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);

        if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUseProgram(0);
    glBindVertexArray(0);

    if (Settings::Instance()->ShowBoundingBox && this->so_selectedYn) {
        glm::mat4 matrixBB = glm::mat4(1.0f);
        matrixBB *= matrixProjection;
        matrixBB *= matrixCamera;
        matrixBB *= matrixModel;
        this->boundingBox->render(matrixBB, this->so_outlineColor);
    }
}

#pragma mark - Scene Options

void Mesh::setOptionsFOV(float fov) {
    this->so_fov = fov;
}

void Mesh::setOptionsSelected(bool selectedYn) {
    this->so_selectedYn = selectedYn;
}

void Mesh::setOptionsOutlineColor(glm::vec4 outlineColor) {
    this->so_outlineColor = outlineColor;
}

void Mesh::setOptionsOutlineThickness(float thickness) {
    this->so_outlineThickness = thickness;
}
