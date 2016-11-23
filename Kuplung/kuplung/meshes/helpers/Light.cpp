//
//  Light.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Light.hpp"

#include <fstream>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Light::~Light() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);
    glDisableVertexAttribArray(this->glAttributeTextureCoord);
    glDisableVertexAttribArray(this->glAttributeVertexNormal);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);

    this->positionX.reset();
    this->positionY.reset();
    this->positionZ.reset();
    this->directionX.reset();
    this->directionY.reset();
    this->directionZ.reset();
    this->scaleX.reset();
    this->scaleY.reset();
    this->scaleZ.reset();
    this->rotateX.reset();
    this->rotateY.reset();
    this->rotateZ.reset();
    this->rotateCenterX.reset();
    this->rotateCenterY.reset();
    this->rotateCenterZ.reset();
    this->ambient.reset();
    this->diffuse.reset();
    this->specular.reset();
    this->lCutOff.reset();
    this->lOuterCutOff.reset();
    this->lConstant.reset();
    this->lLinear.reset();
    this->lQuadratic.reset();

    this->glUtils.reset();
}

#pragma mark - Initialization

void Light::init() {
    this->glUtils = std::make_unique<GLUtils>();
    this->lightDirectionRay = new RayLine();
}

void Light::setModel(MeshModel meshModel) {
    this->meshModel = meshModel;
}

void Light::initProperties(LightSourceType type) {
    this->showLampObject = true;
    this->showLampDirection = true;
    this->showInWire = false;

    this->positionX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->positionY = std::make_unique<ObjectCoordinate>(false, 5.0f);
    this->positionZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->directionX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->directionY = std::make_unique<ObjectCoordinate>(false, 1.0f);
    this->directionZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->scaleX = std::make_unique<ObjectCoordinate>(false, 1.0f);
    this->scaleY = std::make_unique<ObjectCoordinate>(false, 1.0f);
    this->scaleZ = std::make_unique<ObjectCoordinate>(false, 1.0f);

    this->rotateX = std::make_unique<ObjectCoordinate>(false, 0.0f); // -71.0f
    this->rotateY = std::make_unique<ObjectCoordinate>(false, 0.0f); // -36.0f
    this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->rotateCenterX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateCenterY = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateCenterZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->ambient = std::make_unique<MaterialColor>(false, false, 0.3f, glm::vec3(1.0, 1.0, 1.0));
    this->diffuse = std::make_unique<MaterialColor>(false, false, 1.0f, glm::vec3(1.0, 1.0, 1.0));

    switch (type) {
        case LightSourceType_Directional: {
            this->lConstant = std::make_unique<ObjectCoordinate>(false, 0.0f);
            this->lLinear = std::make_unique<ObjectCoordinate>(false, 0.0f);
            this->lQuadratic = std::make_unique<ObjectCoordinate>(false, 0.0f);
            this->specular = std::make_unique<MaterialColor>(false, false, 0.0f, glm::vec3(1.0, 1.0, 1.0));
            this->lCutOff = std::make_unique<ObjectCoordinate>(false, -180.0f);
            this->lOuterCutOff = std::make_unique<ObjectCoordinate>(false, 160.0f);
            break;
        }
        case LightSourceType_Point: {
            this->lConstant = std::make_unique<ObjectCoordinate>(false, 0.0f);
            this->lLinear = std::make_unique<ObjectCoordinate>(false, 0.2f);
            this->lQuadratic = std::make_unique<ObjectCoordinate>(false, 0.05f);
            this->specular = std::make_unique<MaterialColor>(false, false, 0.0f, glm::vec3(1.0, 1.0, 1.0));
            this->lCutOff = std::make_unique<ObjectCoordinate>(false, -180.0f);
            this->lOuterCutOff = std::make_unique<ObjectCoordinate>(false, 160.0f);
            break;
        }
        case LightSourceType_Spot: {
            this->lConstant = std::make_unique<ObjectCoordinate>(false, 1.0f);
            this->lLinear = std::make_unique<ObjectCoordinate>(false, 0.09f);
            this->lQuadratic = std::make_unique<ObjectCoordinate>(false, 0.032f);
            this->specular = std::make_unique<MaterialColor>(false, false, 1.0f, glm::vec3(1.0, 1.0, 1.0));
            this->lCutOff = std::make_unique<ObjectCoordinate>(false, 12.5f);
            this->lOuterCutOff = std::make_unique<ObjectCoordinate>(false, 15.0f);
            break;
        }
        default:
            break;
    }

    this->matrixModel = glm::mat4(1.0);

    glm::vec3 lrFrom = glm::vec3(0);
    glm::vec3 lrTo = glm::vec3(this->positionX->point, this->positionY->point * -100.0f, this->positionZ->point);
    this->lightDirectionRay->init();
    this->lightDirectionRay->initShaderProgram();
    this->lightDirectionRay->initBuffers(lrFrom, lrTo);
}

#pragma mark - Public

bool Light::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/light.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/light.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
        this->glAttributeVertexNormal = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexNormal");
        this->glAttributeTextureCoord = this->glUtils->glGetAttribute(this->shaderProgram, "a_textureCoord");

        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");

        this->glUniformSampler = this->glUtils->glGetUniform(this->shaderProgram, "u_sampler");
        this->glUniformUseColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_useColor");
        this->glUniformColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_color");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void Light::initBuffers(std::string const& assetsFolder) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.countVertices * sizeof(glm::vec3), &this->meshModel.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.countNormals * sizeof(glm::vec3), &this->meshModel.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexNormal);
    glVertexAttribPointer(this->glAttributeVertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // textures and colors
    if (this->meshModel.texture_coordinates.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->meshModel.texture_coordinates.size() * sizeof(glm::vec3), &this->meshModel.texture_coordinates[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeTextureCoord);
        glVertexAttribPointer(this->glAttributeTextureCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

        if (this->meshModel.ModelMaterial.TextureDiffuse.Image != "") {
            std::string matImageLocal = assetsFolder + "/gui/" + this->meshModel.ModelMaterial.TextureDiffuse.Image;

            int tWidth, tHeight, tChannels;
            unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
            if (!tPixels)
                Settings::Instance()->funcDoLog("Can't load diffuse texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
            else {
                glGenTextures(1, &this->vboTextureDiffuse);
                glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
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

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->meshModel.countIndices * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Render

void Light::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera) {
    if (this->glVAO > 0 && this->showLampObject) {
        glUseProgram(this->shaderProgram);

        this->matrixProjection = matrixProjection;
        this->matrixCamera = matrixCamera;

        this->matrixModel = glm::mat4(1.0);
        this->matrixModel = glm::scale(this->matrixModel, glm::vec3(this->scaleX->point, this->scaleY->point, this->scaleZ->point));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

//        glm::vec3 vLightDirection = glm::vec3(this->directionX->point, this->directionY->point, this->directionZ->point);

//        // lamp
//        this->meshLight->render(this->matrixProjection, this->matrixCamera, this->matrixModel);

//        // direction line
//        glm::mat4 mtxModelDot = this->matrixModel;
//        mtxModelDot = glm::rotate(mtxModelDot, glm::radians(90.0f), glm::vec3(1, 0, 0));
//        this->meshLightRay->initBuffers(glm::vec3(0, 0, 0), vLightDirection, true);
//        this->meshLightRay->render(this->matrixProjection, this->matrixCamera, mtxModelDot);

        // texture
        if (this->vboTextureDiffuse > 0)
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);

        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        //glEnable(GL_CULL_FACE);

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        if (this->vboTextureDiffuse == 0) {
            glUniform1i(this->glUniformUseColor, 1);
            glUniform3f(this->glUniformColor, this->meshModel.ModelMaterial.DiffuseColor.r, this->meshModel.ModelMaterial.DiffuseColor.g, this->meshModel.ModelMaterial.DiffuseColor.b);
        }

        // draw
        glBindVertexArray(this->glVAO);
        if (this->showInWire)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);
        if (this->showInWire)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);

        // clear texture
        if (this->vboTextureDiffuse > 0)
            glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);

        this->lightDirectionRay->matrixModel = this->matrixModel;
        if (this->showLampDirection)
            this->lightDirectionRay->render(this->matrixProjection, this->matrixCamera);
    }
}
