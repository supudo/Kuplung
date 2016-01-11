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

#pragma mark - Destroy

MeshModelFace::~MeshModelFace() {
    this->destroy();
}

void MeshModelFace::destroy() {
//    glDeleteBuffers(1, &this->vboVertices);
//    glDeleteBuffers(1, &this->vboNormals);
//    glDeleteBuffers(1, &this->vboTextureCoordinates);
//    glDeleteBuffers(1, &this->vboTextureColor);
//    glDeleteBuffers(1, &this->vboIndices);
//
//    if (this->vboTextureAmbient > 0)
//        glDeleteBuffers(1, &this->vboTextureAmbient);
//    if (this->vboTextureDiffuse > 0)
//        glDeleteBuffers(1, &this->vboTextureDiffuse);
//    if (this->vboTextureSpecular > 0)
//        glDeleteBuffers(1, &this->vboTextureSpecular);
//    if (this->vboTextureShiness > 0)
//        glDeleteBuffers(1, &this->vboTextureShiness);
//    if (this->vboTextureDissolve > 0)
//        glDeleteBuffers(1, &this->vboTextureDissolve);

    glDisableVertexAttribArray(this->glVS_VertexPosition);
    glDisableVertexAttribArray(this->glFS_TextureCoord);
    glDisableVertexAttribArray(this->glVS_VertexNormal);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

void MeshModelFace::init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion) {
    this->doLogFunc = doLog;
    this->glUtils = new GLUtils();
    this->glUtils->init(std::bind(&MeshModelFace::doLog, this, std::placeholders::_1));
    this->shaderName = shaderName;
    this->glslVersion = glslVersion;
    this->so_alpha = -1;
    this->so_lightColor = glm::vec3(0, 0, 0);
    this->so_refraction = this->oFace.faceMaterial.opticalDensity;
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

    // tessellation control shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".tcs";
    std::string shaderTessControlSource = readFile(shaderPath.c_str());
    shaderTessControlSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderTessControlSource;
    const char *shader_tess_control = shaderTessControlSource.c_str();

    // tessellation evaluation shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".tes";
    std::string shaderTessEvalSource = readFile(shaderPath.c_str());
    shaderTessEvalSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderTessEvalSource;
    const char *shader_tess_eval = shaderTessEvalSource.c_str();

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
        this->glGS_GeomDisplacementLocation = this->glUtils->glGetUniform(this->shaderProgram, "vs_displacementLocation");

        this->glFS_AlphaBlending = this->glUtils->glGetUniform(this->shaderProgram, "fs_alpha");
        this->glFS_CameraPosition = this->glUtils->glGetUniform(this->shaderProgram, "fs_cameraPosition");
        this->glFS_OpticalDensity = this->glUtils->glGetUniform(this->shaderProgram, "fs_refraction");

        this->glFS_Light_Position = this->glUtils->glGetUniform(this->shaderProgram, "fs_lightPosition");
        this->glFS_Light_Direction = this->glUtils->glGetUniform(this->shaderProgram, "fs_lightDirection");

        this->glFS_AmbientColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_ambientColor");
        this->glFS_DiffuseColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_diffuseColor");
        this->glFS_SpecularColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_specularColor");

        this->glVS_MVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVPMatrix");
        this->glFS_MMatrix = this->glUtils->glGetUniform(this->shaderProgram, "fs_MMatrix");

        this->glFS_Sampler = this->glUtils->glGetUniform(this->shaderProgram, "fs_sampler");

        this->glFS_ScreenResX = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResX");
        this->glFS_ScreenResY = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResY");
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
    }

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->oFace.indicesCount * sizeof(GLuint), &this->oFace.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Scene Options

void MeshModelFace::setOptionsFOV(float fov) {
    this->so_fov = fov;
}

void MeshModelFace::setOptionsAlpha(float alpha) {
    this->so_alpha = alpha;
}

void MeshModelFace::setOptionsLightPosition(glm::vec3 lightPosition) {
    this->so_lightPosition = lightPosition;
}

void MeshModelFace::setOptionsLightDirection(glm::vec3 lightDirection) {
    this->so_lightDirection = lightDirection;
}

void MeshModelFace::setOptionsLightColor(glm::vec3 lightColor) {
    this->so_lightColor = lightColor;
}

void MeshModelFace::setOptionsDisplacement(glm::vec3 displacement) {
    this->so_displacement = displacement;
}

void MeshModelFace::setOptionsRefraction(float refraction) {
    this->so_refraction = refraction;
}

#pragma mark - Render

void MeshModelFace::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        this->matrixProjection = matrixProjection;
        this->matrixCamera = matrixCamera;
        this->matrixModel = matrixModel;
        this->vecCameraPosition = vecCameraPosition;

        // texture
        if (this->vboTextureDiffuse > 0)
            glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);

        // drawing options
        //glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        //glEnable(GL_CULL_FACE);

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

        // camera position
        glUniform3f(this->glFS_CameraPosition, vecCameraPosition.x, vecCameraPosition.y, vecCameraPosition.z);

        // light
        glUniform3f(this->glFS_Light_Position, this->so_lightPosition.x, this->so_lightPosition.y, this->so_lightPosition.z);
        glUniform3f(this->glFS_Light_Direction, this->so_lightDirection.x, this->so_lightDirection.y, this->so_lightDirection.z);

        // colors
        glUniform3f(this->glFS_AmbientColor, this->oFace.faceMaterial.ambient.r, this->oFace.faceMaterial.ambient.g, this->oFace.faceMaterial.ambient.b);
        glUniform3f(this->glFS_DiffuseColor, this->oFace.faceMaterial.diffuse.r, this->oFace.faceMaterial.diffuse.g, this->oFace.faceMaterial.diffuse.b);
        glUniform3f(this->glFS_SpecularColor, this->oFace.faceMaterial.specular.r, this->oFace.faceMaterial.specular.g, this->oFace.faceMaterial.specular.b);

        // geometry shader displacement
        glUniform3f(this->glGS_GeomDisplacementLocation, this->so_displacement.x, this->so_displacement.y, this->so_displacement.z);

        // Refraction
        glUniform1f(this->glFS_OpticalDensity, this->so_refraction);
        glUniform1f(this->glFS_ScreenResX, Settings::Instance()->SDL_Window_Width);
        glUniform1f(this->glFS_ScreenResY, Settings::Instance()->SDL_Window_Height);

        // draw
        glBindVertexArray(this->glVAO);
        if (Settings::Instance()->wireframesMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, this->oFace.indicesCount, GL_UNSIGNED_INT, nullptr);
        if (Settings::Instance()->wireframesMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);

        // clear texture
        if (this->vboTextureDiffuse > 0)
            glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }
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
