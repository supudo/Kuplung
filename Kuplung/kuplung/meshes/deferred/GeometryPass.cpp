//
//  GeometryPass.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "GeometryPass.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

GeometryPass::GeometryPass() {
}

bool GeometryPass::init() {
    bool success = true;

    this->glUtils = new GLUtils();

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_model_face_geometry.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    // fragment shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_model_face_geometry.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

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
        glValidateProgram(this->shaderProgram);
        glGetProgramiv(this->shaderProgram, GL_VALIDATE_STATUS, &programSuccess);
        if (!programSuccess) {
            GLchar ErrorLog[1024] = { 0 };
            glGetProgramInfoLog(this->shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
            Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[Geometry Pass] Invalid shader program: '%s'\n", ErrorLog));
            return success = false;
        }
        else {
            this->glVS_WVPLocation = this->glUtils->glGetUniform(this->shaderProgram, "vs_MVPMatrix");
            this->glVS_WorldMatrixLocation = this->glUtils->glGetUniform(this->shaderProgram, "vs_ModelMatrix");
            this->glFS_colorTextureUnitLocation = this->glUtils->glGetUniform(this->shaderProgram, "fs_samplerDiffuse");
        }
    }

    glBindVertexArray(0);

    return success;
}

void GeometryPass::setWVP(const glm::mat4& WVP) {
    glUniformMatrix4fv(this->glVS_WVPLocation, 1, GL_TRUE, glm::value_ptr(WVP));
}

void GeometryPass::setWorldMatrix(const glm::mat4& WorldInverse) {
    glUniformMatrix4fv(this->glVS_WorldMatrixLocation, 1, GL_TRUE, glm::value_ptr(WorldInverse));
}

void GeometryPass::setColorTextureUnit(int textureUnit) {
    glUniform1i(this->glFS_colorTextureUnitLocation, textureUnit);
}

void GeometryPass::enable() {
    glUseProgram(this->shaderProgram);
}

std::string GeometryPass::readFile(const char *filePath) {
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
