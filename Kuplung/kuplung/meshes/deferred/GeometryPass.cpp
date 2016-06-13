//
//  GeometryPass.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "GeometryPass.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

GeometryPass::GeometryPass() {
}

bool GeometryPass::init() {
    this->glUtils = new GLUtils();

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/geometry_pass.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/geometry_pass.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return false;
    }
    else {
        this->glVS_MVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "gWVP");
        this->glVS_WorldMatrix = this->glUtils->glGetUniform(this->shaderProgram, "gWorld");
        this->glFS_DiffuseSampler = this->glUtils->glGetUniform(this->shaderProgram, "gColorMap");
    }

    return true;
}

void GeometryPass::setWVP(const glm::mat4& wvp) {
    glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_TRUE, glm::value_ptr(wvp));
}

void GeometryPass::setWorldMatrix(const glm::mat4& worldInverse) {
    glUniformMatrix4fv(this->glVS_WorldMatrix, 1, GL_TRUE, glm::value_ptr(worldInverse));
}


void GeometryPass::setColorTextureUnit(int textureUnit) {
    glUniform1i(this->glFS_DiffuseSampler, textureUnit);
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

