//
//  MeshTerrain.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "MeshTerrain.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma mark - Destroy

MeshTerrain::~MeshTerrain() {
    this->destroy();
}

void MeshTerrain::destroy() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);
    
    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);
    
    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);
    
    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

void MeshTerrain::init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion) {
    this->doLogFunc = doLog;
    this->glUtils = new GLUtils();
    this->glUtils->init(std::bind(&MeshTerrain::doLog, this, std::placeholders::_1));
    this->shaderName = shaderName;
    this->glslVersion = glslVersion;
    this->terrainGenerator = new HeightmapGenerator();
}

#pragma mark - Public

bool MeshTerrain::initShaderProgram() {
    bool success = true;
    
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".vert";
    std::string shaderVertexSource = readFile(shaderPath.c_str());
    shaderVertexSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderVertexSource;
    const char *shader_vertex = shaderVertexSource.c_str();
    
    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".frag";
    std::string shaderFragmentSource = readFile(shaderPath.c_str());
    shaderFragmentSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderFragmentSource;
    const char *shader_fragment = shaderFragmentSource.c_str();
    
    this->shaderProgram = glCreateProgram();
    
    this->shaderVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->shaderVertex, 1, &shader_vertex, NULL);
    glCompileShader(this->shaderVertex);
    
    GLint isShaderVertexCompiled = GL_FALSE;
    glGetShaderiv(this->shaderVertex, GL_COMPILE_STATUS, &isShaderVertexCompiled);
    if (isShaderVertexCompiled != GL_TRUE) {
        this->doLog(Settings::Instance()->string_format("Unable to compile vertex shader %d!\n", this->shaderVertex));
        this->glUtils->printShaderLog(this->shaderVertex);
    }
    else {
        glAttachShader(this->shaderProgram, this->shaderVertex);
        
        this->shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(this->shaderFragment, 1, &shader_fragment, NULL);
        glCompileShader(this->shaderFragment);
        
        GLint isShaderFragmentCompiled = GL_FALSE;
        glGetShaderiv(this->shaderFragment, GL_COMPILE_STATUS, &isShaderFragmentCompiled);
        if (isShaderFragmentCompiled != GL_TRUE) {
            this->doLog(Settings::Instance()->string_format("Unable to compile fragment shader %d!\n", this->shaderFragment));
            this->glUtils->printShaderLog(this->shaderFragment);
            return success = false;
        }
        else {
            glAttachShader(this->shaderProgram, this->shaderFragment);
            glLinkProgram(this->shaderProgram);
            
            GLint programSuccess = GL_TRUE;
            glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
            if (programSuccess != GL_TRUE) {
                this->doLog(Settings::Instance()->string_format("Error linking program %d!\n", this->shaderProgram));
                this->glUtils->printProgramLog(this->shaderProgram);
                return success = false;
            }
            else {
                this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
                this->glAttributeColor = this->glUtils->glGetAttribute(this->shaderProgram, "a_color");
                this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
            }
        }
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return success;
}

void MeshTerrain::initBuffers(std::string assetsFolder) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);
    
    this->terrainGenerator->generateTerrain(assetsFolder, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0, 0);
    this->heightmapImage = this->terrainGenerator->heightmapImage;
    
    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->terrainGenerator->vertices.size() * sizeof(GLfloat), &this->terrainGenerator->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    
    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, this->terrainGenerator->colors.size() * sizeof(GLfloat), &this->terrainGenerator->colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeColor);
    glVertexAttribPointer(this->glAttributeColor, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
    
    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->terrainGenerator->indices.size() * sizeof(GLuint), &this->terrainGenerator->indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

#pragma mark - Render

void MeshTerrain::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);
        
        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        //glEnable(GL_CULL_FACE);
        
        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        
        // draw
        glBindVertexArray(this->glVAO);
        glDrawElements(GL_TRIANGLES, (int)this->terrainGenerator->indices.size(), GL_UNSIGNED_INT, nullptr);
        //glDrawArrays(GL_LINES, 0, (int)this->terrainGenerator->vertices.size());
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string MeshTerrain::readFile(const char *filePath) {
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

void MeshTerrain::doLog(std::string logMessage) {
    this->doLogFunc("[MeshTerrain] " + logMessage);
}
