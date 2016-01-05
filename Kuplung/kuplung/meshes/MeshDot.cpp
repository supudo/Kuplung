//
//  MeshDot.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "MeshDot.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma mark - Destroy

MeshDot::~MeshDot() {
    this->destroy();
}

void MeshDot::destroy() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);
    
    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);
    
    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);
    
    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

void MeshDot::init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion) {
    this->doLogFunc = doLog;
    this->glUtils = new GLUtils();
    this->glUtils->init(std::bind(&MeshDot::doLog, this, std::placeholders::_1));
    this->shaderName = shaderName;
    this->glslVersion = glslVersion;
}

#pragma mark - Public

bool MeshDot::initShaderProgram() {
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
                this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
            }
        }
    }
    
    this->x = 999;
    this->y = 999;
    this->z = 999;
    glGenVertexArrays(1, &this->glVAO);
    
    return success;
}

void MeshDot::initBuffers(glm::vec3 position, glm::vec3 direction, bool simple) {
    if (position.x != x && position.y != y && position.z != z) {
        this->x = position.x;
        this->y = position.y;
        this->z = position.z;

        glBindVertexArray(this->glVAO);
        
        this->axisSize = (simple ? 2 : 12);
        std::vector<GLfloat> vertices;
        if (simple) {
            vertices.push_back(position.x);
            vertices.push_back(position.y);
            vertices.push_back(position.z);

            vertices.push_back(direction.x);
            vertices.push_back(direction.y);
            vertices.push_back(direction.z);
        }
        else {
            GLfloat g_vertex_buffer_data[] = {
                 -0.866024971, 0, 0.5,
                 0.866024971, 0, -0.5,
                 0, 0, -1,
                 0, 0, -1,
                 -0.866024971, 0, -0.5,
                 -0.866024971, 0, 0.5,
                 -0.866024971, 0, 0.5,
                 0, 0, 1,
                 0.866024971, 0, 0.5,
                 0.866024971, 0, 0.5,
                 0.866024971, 0, -0.5,
                 -0.866024971, 0, 0.5
            };
            for (size_t i=0; i<sizeof(g_vertex_buffer_data); i++) {
                vertices.push_back(g_vertex_buffer_data[i]);
            }
        }
        
        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeVertexPosition);
        glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        if (!simple) {
            std::vector<GLuint> indices;
            for (size_t i=0; i<12; i++) {
                indices.push_back((int)i);
            }

            // indices
            glGenBuffers(1, &this->vboIndices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
        }
        
        glBindVertexArray(0);
    }
}

#pragma mark - Render

void MeshDot::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);
        
        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        glLineWidth((GLfloat)5.5f);
        
        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        
        // draw
        glBindVertexArray(this->glVAO);
        if (this->axisSize > 2)
            glDrawElements(GL_TRIANGLES, this->axisSize, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(GL_LINES, 0, this->axisSize);
        
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string MeshDot::readFile(const char *filePath) {
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

void MeshDot::doLog(std::string logMessage) {
    this->doLogFunc("[MeshDot] " + logMessage);
}
