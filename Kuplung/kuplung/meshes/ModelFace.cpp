//
//  ModelFace.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFace.hpp"
#include <fstream>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "kuplung/utilities/stb/stb_image.h"

#include <glm/gtc/type_ptr.hpp>

ModelFace::ModelFace() {
}

ModelFace* ModelFace::clone(int modelID) {
    ModelFace *mmf = new ModelFace();

    mmf->ModelID = modelID;

    mmf->matrixCamera = this->matrixCamera;
    mmf->matrixModel = this->matrixModel;
    mmf->matrixProjection = this->matrixProjection;

    mmf->ModelID = this->ModelID;
    mmf->oFace = this->oFace;

    mmf->so_fov = this->so_fov;
    mmf->so_outlineThickness = this->so_outlineThickness;
    mmf->so_outlineColor = this->so_outlineColor;

    mmf->vecCameraPosition = this->vecCameraPosition;

    mmf->GLSL_LightSource_Number = this->GLSL_LightSource_Number;
    mmf->mfLights = this->mfLights;

    mmf->init(std::bind(&ModelFace::doLog, this, std::placeholders::_1));
    mmf->setModel(mmf->oFace);
    mmf->initShaderProgram();
    mmf->initBuffers(Settings::Instance()->currentFolder);

    return mmf;
}

#pragma mark - Destroy

ModelFace::~ModelFace() {
    this->destroy();
}

void ModelFace::destroy() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboNormals);
    glDeleteBuffers(1, &this->vboTextureCoordinates);
    glDeleteBuffers(1, &this->vboIndices);

    if (this->vboTextureAmbient > 0)
        glDeleteBuffers(1, &this->vboTextureAmbient);
    if (this->vboTextureDiffuse > 0)
        glDeleteBuffers(1, &this->vboTextureDiffuse);
    if (this->vboTextureSpecular > 0)
        glDeleteBuffers(1, &this->vboTextureSpecular);
    if (this->vboTextureSpecularExp > 0)
        glDeleteBuffers(1, &this->vboTextureSpecularExp);
    if (this->vboTextureDissolve > 0)
        glDeleteBuffers(1, &this->vboTextureDissolve);

    GLint param;
    GLuint objName;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
    if (GL_RENDERBUFFER == param) {
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
        objName = ((GLuint*)(&param))[0];
        glDeleteRenderbuffers(1, &objName);
    }
    else if(GL_TEXTURE == param) {
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
        objName = ((GLuint*)(&param))[0];
        glDeleteTextures(1, &objName);
    }

    glDisableVertexAttribArray(this->glVS_VertexPosition);
    glDisableVertexAttribArray(this->glFS_TextureCoord);
    glDisableVertexAttribArray(this->glVS_VertexNormal);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDetachShader(this->shaderProgram, this->shaderGeometry);
//    glDetachShader(this->shaderProgram, this->shaderTessControl);
//    glDetachShader(this->shaderProgram, this->shaderTessEval);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);
    glDeleteShader(this->shaderGeometry);
//    glDeleteShader(this->shaderTessControl);
//    glDeleteShader(this->shaderTessEval);

    glDetachShader(this->shaderProgramReflection, this->shaderVertexReflection);
    glDetachShader(this->shaderProgramReflection, this->shaderFragmentReflection);
    glDeleteProgram(this->shaderProgramReflection);
    glDeleteShader(this->shaderVertexReflection);
    glDeleteShader(this->shaderFragmentReflection);

    glDeleteVertexArrays(1, &this->glVAO);

    this->oFace = {};
}

#pragma mark - Initialization

void ModelFace::init(std::function<void(std::string)> doLog) {
    this->doLogFunc = doLog;
    this->glUtils = new GLUtils();
    this->glUtils->init(std::bind(&ModelFace::doLog, this, std::placeholders::_1));
    this->so_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);

    this->showMaterialEditor = false;
    this->GLSL_LightSource_Number = 8;

    // light
    this->Setting_LightStrengthAmbient = 0.5;
    this->Setting_LightStrengthDiffuse = 1.0;
    this->Setting_LightStrengthSpecular = 0.5;
    this->Setting_LightAmbient = glm::vec3(1.0, 1.0, 1.0);
    this->Setting_LightDiffuse = glm::vec3(1.0, 1.0, 1.0);
    this->Setting_LightSpecular = glm::vec3(1.0, 1.0, 1.0);

    // material
    this->Setting_MaterialRefraction = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ this->oFace.faceMaterial.opticalDensity });
    this->materialAmbient = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialDiffuse = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialSpecular = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialEmission = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
}

void ModelFace::setModel(objModelFace oFace) {
    this->oFace = oFace;
}

void ModelFace::initModelProperties() {
    this->Setting_CelShading = false;
    this->Setting_Alpha = 1.0f;
    this->showMaterialEditor = false;

    this->Settings_Eye = new ObjectEye();
    this->Settings_Eye->View_Eye = glm::vec3(1.0, 1.0, 1.0);
    this->Settings_Eye->View_Center = glm::vec3(0.0, 0.0, 0.0);
    this->Settings_Eye->View_Up = glm::vec3(0.0, 1.0, 0.0);

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->displaceX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->displaceY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->displaceZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->matrixModel = glm::mat4(1.0);

    this->Setting_MaterialRefraction = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ this->oFace.faceMaterial.opticalDensity });
    this->Setting_MaterialSpecularExp = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ this->oFace.faceMaterial.specularExp });

    this->Setting_LightPosition = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDirection = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightAmbient = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDiffuse = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightSpecular = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightStrengthAmbient = 1.0;
    this->Setting_LightStrengthDiffuse = 1.0;
    this->Setting_LightStrengthSpecular = 1.0;

    this->materialIlluminationModel = 1;

    this->materialAmbient = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(this->oFace.faceMaterial.ambient.r, this->oFace.faceMaterial.ambient.g, this->oFace.faceMaterial.ambient.b) });
    this->materialDiffuse = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(this->oFace.faceMaterial.diffuse.r, this->oFace.faceMaterial.diffuse.g, this->oFace.faceMaterial.diffuse.b) });
    this->materialSpecular = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(this->oFace.faceMaterial.specular.r, this->oFace.faceMaterial.specular.g, this->oFace.faceMaterial.specular.b) });
    this->materialEmission = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(this->oFace.faceMaterial.emission.r, this->oFace.faceMaterial.emission.g, this->oFace.faceMaterial.emission.b) });
}

void ModelFace::initProperties() {
    this->Setting_CelShading = false;
    this->Setting_Alpha = 1.0f;

    this->Settings_Eye = new ObjectEye();
    this->Settings_Eye->View_Eye = glm::vec3(1.0, 1.0, 1.0);
    this->Settings_Eye->View_Center = glm::vec3(0.0, 0.0, 0.0);
    this->Settings_Eye->View_Up = glm::vec3(0.0, 1.0, 0.0);

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->displaceX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->displaceY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->displaceZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->matrixModel = glm::mat4(1.0);

    this->Setting_MaterialRefraction = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->Setting_MaterialSpecularExp = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 100.0 });

    this->Setting_LightPosition = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDirection = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightAmbient = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightDiffuse = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightSpecular = glm::vec3(0.0, 0.0, 0.0);
    this->Setting_LightStrengthAmbient = 1.0;
    this->Setting_LightStrengthDiffuse = 1.0;
    this->Setting_LightStrengthSpecular = 1.0;

    this->materialIlluminationModel = 1;
    this->materialAmbient = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialDiffuse = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialSpecular = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->materialEmission = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
}

#pragma mark - Public

bool ModelFace::initShaderProgram() {
    bool success = true;

    // init FBO
    this->fboDefault = 0;

    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

//    // tessellation control shader
//    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.tcs";
//    std::string shaderSourceTCS = readFile(shaderPath.c_str());
//    const char *shader_tess_control = shaderSourceTCS.c_str();

//    // tessellation evaluation shader
//    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.tes";
//    std::string shaderSourceTES = readFile(shaderPath.c_str());
//    const char *shader_tess_eval = shaderSourceTES.c_str();

    // geometry shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.geom";
    std::string shaderSourceGeometry = readFile(shaderPath.c_str());
    const char *shader_geometry = shaderSourceGeometry.c_str();

    // fragment shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/model_face.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
//    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderTessControl, GL_TESS_CONTROL_SHADER, shader_tess_control);
//    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderTessEval, GL_TESS_EVALUATION_SHADER, shader_tess_eval);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderGeometry, GL_GEOMETRY_SHADER, shader_geometry);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        this->doLog(Settings::Instance()->string_format("Error linking program %d!\n", this->shaderProgram));
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glVS_VertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexPosition");
        this->glFS_TextureCoord = this->glUtils->glGetAttribute(this->shaderProgram, "vs_textureCoord");
        this->glVS_VertexNormal = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexNormal");

        // misc
        this->glGS_GeomDisplacementLocation = this->glUtils->glGetUniform(this->shaderProgram, "vs_displacementLocation");

        this->glFS_AlphaBlending = this->glUtils->glGetUniform(this->shaderProgram, "fs_alpha");
        this->glFS_CelShading = this->glUtils->glGetUniform(this->shaderProgram, "fs_celShading");
        this->glFS_CameraPosition = this->glUtils->glGetUniform(this->shaderProgram, "fs_cameraPosition");
        this->glVS_IsBorder = this->glUtils->glGetUniform(this->shaderProgram, "vs_isBorder");
        this->glFS_OutlineColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_outlineColor");

        this->glVS_MVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVPMatrix");
        this->glFS_MMatrix = this->glUtils->glGetUniform(this->shaderProgram, "fs_MMatrix");

        this->glFS_ScreenResX = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResX");
        this->glFS_ScreenResY = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResY");

        // light
        for (int i=0; i<this->GLSL_LightSource_Number; i++) {
            ModelFace_LightSource *f = new ModelFace_LightSource();
            f->glLight_InUse = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].inUse").c_str());

            f->glLight_Position = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].position").c_str());
            f->glLight_Direction = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].direction").c_str());

            f->glLight_Ambient = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].ambient").c_str());
            f->glLight_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].diffuse").c_str());
            f->glLight_Specular = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].specular").c_str());

            f->glLight_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].strengthAmbient").c_str());
            f->glLight_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].strengthDiffuse").c_str());
            f->glLight_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram, ("directionalLights[" + std::to_string(i) + "].strengthSpecular").c_str());
            this->mfLights.push_back(f);
        }

        // material
        this->glMaterial_Refraction = this->glUtils->glGetUniform(this->shaderProgram, "material.refraction");
        this->glMaterial_SpecularExp = this->glUtils->glGetUniform(this->shaderProgram, "material.specularExp");
        this->glMaterial_IlluminationModel = this->glUtils->glGetUniform(this->shaderProgram, "material.illumination_model");

        this->glMaterial_Ambient = this->glUtils->glGetUniform(this->shaderProgram, "material.ambient");
        this->glMaterial_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, "material.diffuse");
        this->glMaterial_Specular = this->glUtils->glGetUniform(this->shaderProgram, "material.specular");
        this->glMaterial_Emission = this->glUtils->glGetUniform(this->shaderProgram, "material.emission");

        this->glMaterial_SamplerAmbient = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_ambient");
        this->glMaterial_SamplerDiffuse = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_diffuse");
        this->glMaterial_SamplerSpecular = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_specular");
        this->glMaterial_SamplerSpecularExp = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_specularExp");
        this->glMaterial_SamplerDissolve = this->glUtils->glGetUniform(this->shaderProgram, "material.sampler_dissolve");

        this->glMaterial_HasTextureAmbient = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_ambient");
        this->glMaterial_HasTextureDiffuse = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_diffuse");
        this->glMaterial_HasTextureSpecular = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_specular");
        this->glMaterial_HasTextureSpecularExp = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_specularExp");
        this->glMaterial_HasTextureDissolve = this->glUtils->glGetUniform(this->shaderProgram, "material.has_texture_dissolve");
    }

    success |= this->reflectionInit();

    return success;
}

void ModelFace::initBuffers(std::string assetsFolder) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->oFace.verticesCount * sizeof(GLfloat), &this->oFace.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glVS_VertexPosition);
    glVertexAttribPointer(this->glVS_VertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->oFace.normalsCount * sizeof(GLfloat), &this->oFace.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glVS_VertexNormal);
    glVertexAttribPointer(this->glVS_VertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // textures and colors
    if (this->oFace.texture_coordinates.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->oFace.texture_coordinates.size() * sizeof(GLfloat), &this->oFace.texture_coordinates[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glFS_TextureCoord);
        glVertexAttribPointer(this->glFS_TextureCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

        // ambient texture image
        if (this->oFace.faceMaterial.textures_ambient.image != "") {
            std::string matImageLocal = assetsFolder + "/" + this->oFace.faceMaterial.textures_ambient.image;

            int tWidth, tHeight, tChannels;
            unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
            if (!tPixels)
                this->doLog("Can't load ambient texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
            else {
                glGenTextures(1, &this->vboTextureAmbient);
                glBindTexture(GL_TEXTURE_2D, this->vboTextureAmbient);
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

        // diffuse texture image
        if (this->oFace.faceMaterial.textures_diffuse.image != "") {
            std::string matImageLocal = assetsFolder + "/" + this->oFace.faceMaterial.textures_diffuse.image;

            int tWidth, tHeight, tChannels;
            unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
            if (!tPixels)
                this->doLog("Can't load diffuse texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
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

        // specular texture image
        if (this->oFace.faceMaterial.textures_specular.image != "") {
            std::string matImageLocal = assetsFolder + "/" + this->oFace.faceMaterial.textures_specular.image;

            int tWidth, tHeight, tChannels;
            unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
            if (!tPixels)
                this->doLog("Can't load specular texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
            else {
                glGenTextures(1, &this->vboTextureSpecular);
                glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecular);
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

        // specular-exp texture image
        if (this->oFace.faceMaterial.textures_specularExp.image != "") {
            std::string matImageLocal = assetsFolder + "/" + this->oFace.faceMaterial.textures_specularExp.image;

            int tWidth, tHeight, tChannels;
            unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
            if (!tPixels)
                this->doLog("Can't load specular-exp texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
            else {
                glGenTextures(1, &this->vboTextureSpecularExp);
                glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecularExp);
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

        // dissolve texture image
        if (this->oFace.faceMaterial.textures_dissolve.image != "") {
            std::string matImageLocal = assetsFolder + "/" + this->oFace.faceMaterial.textures_dissolve.image;

            int tWidth, tHeight, tChannels;
            unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
            if (!tPixels)
                this->doLog("Can't load dissolve texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
            else {
                glGenTextures(1, &this->vboTextureDissolve);
                glBindTexture(GL_TEXTURE_2D, this->vboTextureDissolve);
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->oFace.indicesCount * sizeof(GLuint), &this->oFace.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Reflection

bool ModelFace::reflectionInit() {
    this->reflectWidth = 512;
    this->reflectHeight = 512;

    GLuint colorTexture;

    // Create a texture object to apply to model
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    // Set up filter and wrap modes for this texture object
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Mipmap generation is not accelerated on iOS so we cannot enable trilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Allocate a texture image with which we can render to
    // Pass NULL for the data parameter since we don't need to load image data.
    // We will be generating the image by rendering to this texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->reflectWidth, this->reflectHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    GLuint depthRenderbuffer;
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, this->reflectWidth, this->reflectHeight);

    glGenFramebuffers(1, &this->fboReflection);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fboReflection);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        this->doLog("Failed to make complete framebuffer object " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboReflection);

    GLint iReflectTexName;

    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &iReflectTexName);

    this->reflectTexName = ((GLuint*)(&iReflectTexName))[0];

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/reflection.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/reflection.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgramReflection = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgramReflection, this->shaderVertexReflection, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgramReflection, this->shaderFragmentReflection, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgramReflection);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgramReflection, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        this->doLog(Settings::Instance()->string_format("Error linking reflection program %d!\n", this->shaderProgramReflection));
        this->glUtils->printProgramLog(this->shaderProgramReflection);
        return false;
    }
    else {
        this->reflectModelViewUniformIdx = glGetUniformLocation(this->shaderProgramReflection, "modelViewMatrix");
        this->reflectProjectionUniformIdx = glGetUniformLocation(this->shaderProgramReflection, "modelViewProjectionMatrix");
        this->reflectNormalMatrixUniformIdx = glGetUniformLocation(this->shaderProgramReflection, "normalMatrix");
    }

    return true;
}

#pragma mark - Render

void ModelFace::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
    this->matrixModel = matrixModel;
    this->vecCameraPosition = vecCameraPosition;
    this->grid = grid;

    if (this->grid->actAsMirror) {
        glCullFace(GL_FRONT);
        this->renderReflectFBO();
    }

    if (!this->grid->actAsMirror) {
        //glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glBindFramebuffer(GL_FRAMEBUFFER, this->fboDefault);
        glBindVertexArray(this->glVAO);
    }
    this->renderModel();

    if (this->grid->actAsMirror)
        this->renderMirrorSurface();

    glUseProgram(0);
    glBindVertexArray(0);
}

void ModelFace::renderReflectFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fboReflection);

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, this->reflectWidth, this->reflectHeight);

//    glm::mat4 mtxModel = this->matrixModel;
//    mtxModel = glm::scale(mtxModel, glm::vec3(1, -1, -1));
//    mtxModel = glm::translate(mtxModel, glm::vec3(0, 300, -800));
//    mtxModel = glm::rotate(mtxModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));

    glm::mat4 mtxModel = this->matrixModel;

    this->matrixModel = glm::scale(this->matrixModel, glm::vec3(1, -1, -1));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 2.5, -2.5));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;

    glUseProgram(this->shaderProgram);

    glUniformMatrix4fv(this->reflectProjectionUniformIdx, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glBindVertexArray(this->glVAO);
    glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
    glCullFace(GL_FRONT);
    glDrawElements(GL_TRIANGLES, this->oFace.indicesCount, GL_UNSIGNED_INT, nullptr);

    this->matrixModel = mtxModel;
//    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 1.5, -4.5));
//    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
//    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(45.0f), glm::vec3(0.7, 0.3, 1));

    glBindFramebuffer(GL_FRAMEBUFFER, this->fboDefault);
    glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
}

void ModelFace::renderMirrorSurface() {
    glBindVertexArray(this->reflectVAO);
    glUseProgram(this->shaderProgramReflection);

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, this->reflectWidth, this->reflectHeight);

    glm::mat4 mtxModel = glm::translate(this->matrixModel, glm::vec3(0.0, -5.0, -2.5));
    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;

    glUniformMatrix4fv(this->reflectModelViewUniformIdx, 1, GL_FALSE, glm::value_ptr(mtxModel));
    glUniformMatrix4fv(this->reflectProjectionUniformIdx, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // The inverse transpose of the top left 3x3 portion of the modelview matrix
    glm::mat3 matrixNormal = glm::mat3(mtxModel);
    glUniformMatrix3fv(this->reflectNormalMatrixUniformIdx, 1, GL_FALSE, glm::value_ptr(matrixNormal));

    //glBindTexture(GL_TEXTURE_2D, this->reflectTexName);
    glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
    glGenerateMipmap(GL_TEXTURE_2D);
    glDrawElements(GL_TRIANGLES, this->oFace.indicesCount, GL_UNSIGNED_INT, nullptr);
}

void ModelFace::renderModel() {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixModel));

        // blending
        if (this->oFace.faceMaterial.transparency < 1.0 || this->Setting_Alpha < 1.0) {
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            if (this->oFace.faceMaterial.transparency < 1.0)
                glUniform1f(this->glFS_AlphaBlending, this->oFace.faceMaterial.transparency);
            else
                glUniform1f(this->glFS_AlphaBlending, this->Setting_Alpha);
        }
        else {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUniform1f(this->glFS_AlphaBlending, 1.0);
        }

        // cel-shading
        glUniform1i(this->glFS_CelShading, this->Setting_CelShading);

        // camera position
        glUniform3f(this->glFS_CameraPosition, this->vecCameraPosition.x, this->vecCameraPosition.y, this->vecCameraPosition.z);

        // screen size
        glUniform1f(this->glFS_ScreenResX, Settings::Instance()->SDL_Window_Width);
        glUniform1f(this->glFS_ScreenResY, Settings::Instance()->SDL_Window_Height);

        // Outline color
        glUniform3f(this->glFS_OutlineColor, this->so_outlineColor.r, this->so_outlineColor.g, this->so_outlineColor.b);

        // geometry shader displacement
        glUniform3f(this->glGS_GeomDisplacementLocation, this->displaceX->point, this->displaceY->point, this->displaceZ->point);

        // lights
        for (int j=0; j<(int)this->lightSources.size(); j++) {
            if (j < this->GLSL_LightSource_Number) {
                Light *light = this->lightSources[j];
                ModelFace_LightSource *f = this->mfLights[j];

                glUniform1i(f->glLight_InUse, 1);

                // light
                glUniform3f(f->glLight_Direction, light->positionX->point, light->positionY->point, light->positionZ->point);
                glUniform3f(f->glLight_Position, light->matrixModel[3].x, light->matrixModel[3].y, light->matrixModel[3].z);

                // color
                glUniform3f(f->glLight_Ambient, light->ambient->color.r, light->ambient->color.g, light->ambient->color.b);
                glUniform3f(f->glLight_Diffuse, light->diffuse->color.r, light->diffuse->color.g, light->diffuse->color.b);
                glUniform3f(f->glLight_Specular, light->specular->color.r, light->specular->color.g, light->specular->color.b);

                // light factors
                glUniform1f(f->glLight_StrengthAmbient, light->ambient->strength);
                glUniform1f(f->glLight_StrengthDiffuse, light->diffuse->strength);
                glUniform1f(f->glLight_StrengthSpecular, light->specular->strength);
            }
        }

        if ((int)this->lightSources.size() < this->GLSL_LightSource_Number) {
            glm::vec3 defLight = glm::vec3(0.0f, 0.0f, 0.0f);
            float defLightStrength = 0.0f;
            for (int j=(int)this->lightSources.size(); j<this->GLSL_LightSource_Number; j++) {
                ModelFace_LightSource *f = this->mfLights[j];

                glUniform1i(f->glLight_InUse, 0);

                // light
                glUniform3f(f->glLight_Direction, defLight.x, defLight.y, defLight.z);
                glUniform3f(f->glLight_Position, defLight.x, defLight.y, defLight.z);

                // color
                glUniform3f(f->glLight_Ambient, defLight.x, defLight.y, defLight.z);
                glUniform3f(f->glLight_Diffuse, defLight.x, defLight.y, defLight.z);
                glUniform3f(f->glLight_Specular, defLight.x, defLight.y, defLight.z);

                // light factors
                glUniform1f(f->glLight_StrengthAmbient, defLightStrength);
                glUniform1f(f->glLight_StrengthDiffuse, defLightStrength);
                glUniform1f(f->glLight_StrengthSpecular, defLightStrength);
            }
        }

        // material
        glUniform1f(this->glMaterial_Refraction, this->Setting_MaterialRefraction->point);
        glUniform1f(this->glMaterial_SpecularExp, this->Setting_MaterialSpecularExp->point);
        glUniform1i(this->glMaterial_IlluminationModel, this->materialIlluminationModel);
        glUniform3f(this->glMaterial_Ambient, this->materialAmbient->color.r, this->materialAmbient->color.g, this->materialAmbient->color.b);
        glUniform3f(this->glMaterial_Diffuse, this->materialDiffuse->color.r, this->materialDiffuse->color.g, this->materialDiffuse->color.b);
        glUniform3f(this->glMaterial_Specular, this->materialSpecular->color.r, this->materialSpecular->color.g, this->materialSpecular->color.b);
        glUniform3f(this->glMaterial_Emission, this->materialEmission->color.r, this->materialEmission->color.g, this->materialEmission->color.b);

        if (this->vboTextureAmbient > 0) {
            glUniform1i(this->glMaterial_SamplerAmbient, 1);
            glUniform1i(this->glMaterial_HasTextureAmbient, 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureAmbient);
        }
        else
            glUniform1i(this->glMaterial_HasTextureAmbient, 0);

        if (this->vboTextureDiffuse > 0) {
            glUniform1i(this->glMaterial_HasTextureDiffuse, 1);
            glUniform1i(this->glMaterial_SamplerDiffuse, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDiffuse, 0);

        if (this->vboTextureSpecular > 0) {
            glUniform1i(this->glMaterial_HasTextureSpecular, 1);
            glUniform1i(this->glMaterial_SamplerSpecular, 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecular);
        }
        else
            glUniform1i(this->glMaterial_HasTextureSpecular, 0);

        if (this->vboTextureSpecularExp > 0) {
            glUniform1i(this->glMaterial_HasTextureSpecularExp, 1);
            glUniform1i(this->glMaterial_SamplerSpecularExp, 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecularExp);
        }
        else
            glUniform1i(this->glMaterial_HasTextureSpecularExp, 0);

        if (this->vboTextureDissolve > 0) {
            glUniform1i(this->glMaterial_HasTextureDissolve, 1);
            glUniform1i(this->glMaterial_SamplerDissolve, 4);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDissolve);
        }
        else
            glUniform1i(this->glMaterial_HasTextureDissolve, 0);

        // outlining
        this->drawOutline();

        // clear texture
//        if (this->vboTextureDiffuse > 0)
//            glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void ModelFace::drawOutline() {
    glm::mat4 mvpMatrix, mtxModel;
    glUniform1f(this->glVS_IsBorder, 0.0);

    if (this->so_selectedYn) {
        glDisable(GL_DEPTH_TEST);
        glUniform1f(this->glVS_IsBorder, 1.0);
        mtxModel = glm::scale(this->matrixModel, glm::vec3(this->so_outlineThickness, this->so_outlineThickness, this->so_outlineThickness));
        mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(mtxModel));
        this->drawOnly();
        glEnable(GL_DEPTH_TEST);
    }

    // model draw
    glUniform1f(this->glVS_IsBorder, 0.0);
    mtxModel = glm::scale(this->matrixModel, glm::vec3(1.0, 1.0, 1.0));
    mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;
    glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(mtxModel));
    this->drawOnly();
}

void ModelFace::drawOnly() {
    if (Settings::Instance()->wireframesMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, this->oFace.indicesCount, GL_UNSIGNED_INT, nullptr);

    if (Settings::Instance()->wireframesMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
        this->doLog("Could not read file " + std::string(filePath) + ". File does not exist.");
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

void ModelFace::doLog(std::string logMessage) {
    this->doLogFunc("[ModelFace] " + logMessage);
}
