//
//  MeshModelFace.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "MeshModelFace.hpp"
#include <fstream>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "utilities/stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

MeshModelFace::MeshModelFace() {
}

MeshModelFace* MeshModelFace::clone(int modelID) {
    MeshModelFace *mmf = new MeshModelFace();

    mmf->ModelID = modelID;

    mmf->matrixCamera = this->matrixCamera;
    mmf->matrixModel = this->matrixModel;
    mmf->matrixProjection = this->matrixProjection;

    mmf->ModelID = this->ModelID;
    mmf->oFace = this->oFace;

    mmf->vecCameraPosition = this->vecCameraPosition;

    mmf->init(std::bind(&MeshModelFace::doLog, this, std::placeholders::_1), Settings::Instance()->ShaderName, Settings::Instance()->OpenGL_GLSL_Version);
    mmf->setModel(mmf->oFace);
    mmf->initShaderProgram();
    mmf->initBuffers(Settings::Instance()->currentFolder);

    return mmf;
}

#pragma mark - Destroy

MeshModelFace::~MeshModelFace() {
    this->destroy();
}

void MeshModelFace::destroy() {
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

    glDeleteVertexArrays(1, &this->glVAO);

    this->oFace = {};
}

#pragma mark - Initialization

void MeshModelFace::init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion) {
    this->doLogFunc = doLog;
    this->glUtils = new GLUtils();
    this->glUtils->init(std::bind(&MeshModelFace::doLog, this, std::placeholders::_1));
    this->shaderName = shaderName;
    this->glslVersion = glslVersion;
    this->so_alpha = -1;
    this->so_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);

    // light
    this->so_lightStrengthAmbient = 0.5;
    this->so_lightStrengthDiffuse = 1.0;
    this->so_lightStrengthSpecular = 0.5;
    this->so_lightAmbient = glm::vec3(1.0, 1.0, 1.0);
    this->so_lightDiffuse = glm::vec3(1.0, 1.0, 1.0);
    this->so_lightSpecular = glm::vec3(1.0, 1.0, 1.0);

    // material
    this->so_materialRefraction = this->oFace.faceMaterial.opticalDensity;
    this->so_materialAmbient = glm::vec3(1.0, 1.0, 1.0);
    this->so_materialDiffuse = glm::vec3(1.0, 1.0, 1.0);
    this->so_materialSpecular = glm::vec3(1.0, 1.0, 1.0);
    this->so_materialEmission = glm::vec3(0, 0, 0);
}

void MeshModelFace::setModel(objModelFace oFace) {
    this->oFace = oFace;
}

#pragma mark - Public

bool MeshModelFace::initShaderProgram() {
    bool success = true;

    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".vert";
    std::string shaderVertexSource = readFile(shaderPath.c_str());
    shaderVertexSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderVertexSource;
    const char *shader_vertex = shaderVertexSource.c_str();

//    // tessellation control shader
//    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".tcs";
//    std::string shaderTessControlSource = readFile(shaderPath.c_str());
//    shaderTessControlSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderTessControlSource;
//    const char *shader_tess_control = shaderTessControlSource.c_str();

//    // tessellation evaluation shader
//    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".tes";
//    std::string shaderTessEvalSource = readFile(shaderPath.c_str());
//    shaderTessEvalSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderTessEvalSource;
//    const char *shader_tess_eval = shaderTessEvalSource.c_str();

    // geometry shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".geom";
    std::string shaderGeometrySource = readFile(shaderPath.c_str());
    shaderGeometrySource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderGeometrySource;
    const char *shader_geometry = shaderGeometrySource.c_str();

    // fragment shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".frag";
    std::string shaderFragmentSource = readFile(shaderPath.c_str());
    shaderFragmentSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderFragmentSource;
    const char *shader_fragment = shaderFragmentSource.c_str();

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
        this->glLight_Position = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].position");
        this->glLight_Direction = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].direction");

        this->glLight_Ambient = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].ambient");
        this->glLight_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].diffuse");
        this->glLight_Specular = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].specular");

        this->glLight_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].strengthAmbient");
        this->glLight_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].strengthDiffuse");
        this->glLight_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram, "directionalLight[0].strengthSpecular");

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

    return success;
}

void MeshModelFace::initBuffers(std::string assetsFolder) {
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

#pragma mark - Render

void MeshModelFace::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        this->matrixProjection = matrixProjection;
        this->matrixCamera = matrixCamera;
        this->matrixModel = matrixModel;
        this->vecCameraPosition = vecCameraPosition;

        // drawing options
        //glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));

        // blending
        if (this->oFace.faceMaterial.transparency < 1.0 || this->so_alpha < 1.0) {
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            if (this->oFace.faceMaterial.transparency < 1.0)
                glUniform1f(this->glFS_AlphaBlending, this->oFace.faceMaterial.transparency);
            else
                glUniform1f(this->glFS_AlphaBlending, this->so_alpha);
        }
        else {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUniform1f(this->glFS_AlphaBlending, 1.0);
        }

        // cel-shading
        glUniform1i(this->glFS_CelShading, this->so_celShading);

        // camera position
        glUniform3f(this->glFS_CameraPosition, vecCameraPosition.x, vecCameraPosition.y, vecCameraPosition.z);

        // screen size
        glUniform1f(this->glFS_ScreenResX, Settings::Instance()->SDL_Window_Width);
        glUniform1f(this->glFS_ScreenResY, Settings::Instance()->SDL_Window_Height);

        // Outline color
        glUniform3f(this->glFS_OutlineColor, this->so_outlineColor.r, this->so_outlineColor.g, this->so_outlineColor.b);

        // geometry shader displacement
        glUniform3f(this->glGS_GeomDisplacementLocation, this->so_displacement.x, this->so_displacement.y, this->so_displacement.z);

        // light
        glUniform3f(this->glLight_Position, this->so_lightPosition.x, this->so_lightPosition.y, this->so_lightPosition.z);
        glUniform3f(this->glLight_Direction, this->so_lightDirection.x, this->so_lightDirection.y, this->so_lightDirection.z);
        // color
        glUniform3f(this->glLight_Ambient, this->so_lightAmbient.r, this->so_lightAmbient.g, this->so_lightAmbient.b);
        glUniform3f(this->glLight_Diffuse, this->so_lightDiffuse.r, this->so_lightDiffuse.g, this->so_lightDiffuse.b);
        glUniform3f(this->glLight_Specular, this->so_lightSpecular.r, this->so_lightSpecular.g, this->so_lightSpecular.b);
        // light factors
        glUniform1f(this->glLight_StrengthAmbient, this->so_lightStrengthAmbient);
        glUniform1f(this->glLight_StrengthDiffuse, this->so_lightStrengthDiffuse);
        glUniform1f(this->glLight_StrengthSpecular, this->so_lightStrengthSpecular);

        // material
        glUniform1f(this->glMaterial_Refraction, this->so_materialRefraction);
        glUniform1f(this->glMaterial_SpecularExp, this->so_materialSpecularExp);
        glUniform1i(this->glMaterial_IlluminationModel, this->so_materialIlluminationModel);
        glUniform3f(this->glMaterial_Ambient, this->so_materialAmbient.r, this->so_materialAmbient.g, this->so_materialAmbient.b);
        glUniform3f(this->glMaterial_Diffuse, this->so_materialDiffuse.r, this->so_materialDiffuse.g, this->so_materialDiffuse.b);
        glUniform3f(this->glMaterial_Specular, this->so_materialSpecular.r, this->so_materialSpecular.g, this->so_materialSpecular.b);
        glUniform3f(this->glMaterial_Emission, this->so_materialEmission.r, this->so_materialEmission.g, this->so_materialEmission.b);

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
        //this->drawOnly();
        //this->outlineOne();
        //this->outlineTwo();
        this->outlineThree();

        // clear texture
//        if (this->vboTextureDiffuse > 0)
//            glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }
}

void MeshModelFace::outlineThree() {
    glm::mat4 mvpMatrix, mtxModel;
    glUniform1f(this->glVS_IsBorder, 0.0);

    //mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;

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

void MeshModelFace::outlineTwo() {
    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;

    glm::mat4 mtxModel;

    if (this->so_selectedYn) {
        // outline
        glDisable(GL_DEPTH_TEST);
        glUniform1f(this->glVS_IsBorder, 1.0);

        mtxModel = glm::scale(this->matrixModel, glm::vec3(this->so_outlineThickness, this->so_outlineThickness, this->so_outlineThickness));
        mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;

        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixModel));

        this->drawOnly();
        glEnable(GL_DEPTH_TEST);
    }

    // model
    glUniform1f(this->glVS_IsBorder, 0.0);

    mtxModel = glm::scale(this->matrixModel, glm::vec3(1.0, 1.0, 1.0));
    mvpMatrix = this->matrixProjection * this->matrixCamera * mtxModel;

    glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixModel));

    this->drawOnly();
}

void MeshModelFace::outlineOne() {
    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;

    // 1st pass
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    // draw
    glUniform1f(this->glVS_IsBorder, 0.0);
    this->drawOnly();

    // 2nd pass
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    glUniform1f(this->glVS_IsBorder, 1.0);
    matrixModel = glm::scale(matrixModel, glm::vec3(this->so_outlineThickness, this->so_outlineThickness, this->so_outlineThickness));
    mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;
    glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(this->glFS_MMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixModel));

    // draw
    this->drawOnly();

    glStencilMask(0xFF);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
}

void MeshModelFace::drawOnly() {
    glBindVertexArray(this->glVAO);
    if (Settings::Instance()->wireframesMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, this->oFace.indicesCount, GL_UNSIGNED_INT, nullptr);
    if (Settings::Instance()->wireframesMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);
}

#pragma mark - Scene Options

void MeshModelFace::setOptionsFOV(float fov) {
    this->so_fov = fov;
}

void MeshModelFace::setOptionsAlpha(float alpha) {
    this->so_alpha = alpha;
}

void MeshModelFace::setOptionsCelShading(bool val) {
    this->so_celShading = val;
}

void MeshModelFace::setOptionsDisplacement(glm::vec3 displacement) {
    this->so_displacement = displacement;
}

void MeshModelFace::setOptionsSelected(bool selectedYn) {
    this->so_selectedYn = selectedYn;
}

void MeshModelFace::setOptionsOutlineColor(glm::vec4 outlineColor) {
    this->so_outlineColor = outlineColor;
}

void MeshModelFace::setOptionsOutlineThickness(float thickness) {
    this->so_outlineThickness = thickness;
}

// light
void MeshModelFace::setOptionsLightPosition(glm::vec3 lightPosition) {
    this->so_lightPosition = lightPosition;
}

void MeshModelFace::setOptionsLightDirection(glm::vec3 lightDirection) {
    this->so_lightDirection = lightDirection;
}

void MeshModelFace::setOptionsLightAmbient(glm::vec3 lightColor) {
    this->so_lightAmbient = lightColor;
}

void MeshModelFace::setOptionsLightDiffuse(glm::vec3 lightColor) {
    this->so_lightDiffuse = lightColor;
}

void MeshModelFace::setOptionsLightSpecular(glm::vec3 lightColor) {
    this->so_lightSpecular = lightColor;
}

void MeshModelFace::setOptionsLightStrengthAmbient(float val) {
    this->so_lightStrengthAmbient = val;
}

void MeshModelFace::setOptionsLightStrengthDiffuse(float val) {
    this->so_lightStrengthDiffuse = val;
}

void MeshModelFace::setOptionsLightStrengthSpecular(float val) {
    this->so_lightStrengthSpecular = val;
}

// material

void MeshModelFace::setOptionsMaterialRefraction(float val) {
    this->so_materialRefraction = val;
}

void MeshModelFace::setOptionsMaterialSpecularExp(float val) {
    this->so_materialSpecularExp = val;
}

void MeshModelFace::setOptionsMaterialIlluminationModel(float val) {
    this->so_materialIlluminationModel = val;
}

void MeshModelFace::setOptionsMaterialAmbient(glm::vec3 lightColor) {
    this->so_materialAmbient = lightColor;
}

void MeshModelFace::setOptionsMaterialDiffuse(glm::vec3 lightColor) {
    this->so_materialDiffuse = lightColor;
}

void MeshModelFace::setOptionsMaterialSpecular(glm::vec3 lightColor) {
    this->so_materialSpecular = lightColor;
}

void MeshModelFace::setOptionsMaterialEmission(glm::vec3 lightColor) {
    this->so_materialEmission = lightColor;
}

#pragma mark - Utilities

std::string MeshModelFace::readFile(const char *filePath) {
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

void MeshModelFace::doLog(std::string logMessage) {
    this->doLogFunc("[MeshModelFace] " + logMessage);
}
