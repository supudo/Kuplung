//
//  GLUtils.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "GLUtils.hpp"
#include <fstream>

void GLUtils::init(std::function<void(std::string)> doLog) {
    this->doLog = doLog;
}

GLuint GLUtils::initShaderProgram(std::string shaderVertexName, std::string shaderFragmentName, int glslVersion) {
    GLuint shaderProgram = 0;
    
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/" + shaderVertexName + ".vert";
    std::string shaderVertexSource = readFile(shaderPath.c_str());
    shaderVertexSource = "#version " + std::to_string(glslVersion) + "\n" + shaderVertexSource;
    const char *shader_vertex = shaderVertexSource.c_str();
    
    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + shaderFragmentName + ".frag";
    std::string shaderFragmentSource = readFile(shaderPath.c_str());
    shaderFragmentSource = "#version " + std::to_string(glslVersion) + "\n" + shaderFragmentSource;
    const char *shader_fragment = shaderFragmentSource.c_str();
    
    shaderProgram = glCreateProgram();
    
    GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderVertex, 1, &shader_vertex, NULL);
    glCompileShader(shaderVertex);
    
    GLint isShaderVertexCompiled = GL_FALSE;
    glGetShaderiv(shaderVertex, GL_COMPILE_STATUS, &isShaderVertexCompiled);
    if (isShaderVertexCompiled != GL_TRUE) {
        this->glDoLog(Settings::Instance()->string_format("Unable to compile vertex shader %d!\n", shaderVertex));
        this->printShaderLog(shaderVertex);
    }
    else {
        glAttachShader(shaderProgram, shaderVertex);
        
        GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shaderFragment, 1, &shader_fragment, NULL);
        glCompileShader(shaderFragment);
        
        GLint isShaderFragmentCompiled = GL_FALSE;
        glGetShaderiv(shaderFragment, GL_COMPILE_STATUS, &isShaderFragmentCompiled);
        if (isShaderFragmentCompiled != GL_TRUE) {
            this->glDoLog(Settings::Instance()->string_format("Unable to compile fragment shader %d!\n", shaderFragment));
            this->printShaderLog(shaderFragment);
        }
        else {
            glAttachShader(shaderProgram, shaderFragment);
            glLinkProgram(shaderProgram);
            
            GLint programSuccess = GL_TRUE;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programSuccess);
            if (programSuccess != GL_TRUE) {
                this->glDoLog(Settings::Instance()->string_format("Error linking program %d!\n", shaderProgram));
                this->printProgramLog(shaderProgram);
            }
        }
    }
    
    return shaderProgram;
}

GLint GLUtils::glGetAttribute(GLuint program, const char* var_name) {
    GLint var = glGetAttribLocation(program, var_name);
    if (var == -1)
        this->glDoLog("Cannot fetch shader attribute - " + std::string(var_name));
    return var;
}

GLint GLUtils::glGetUniform(GLuint program, const char* var_name) {
    GLint var = glGetUniformLocation(program, var_name);
    if (var == -1)
        this->glDoLog("Cannot fetch shader uniform - " + std::string(var_name));
    return var;
}

void GLUtils::printProgramLog(GLuint program) {
    if (glIsProgram(program)) {
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        char* infoLog = new char[ maxLength ];
        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0)
            this->glDoLog(Settings::Instance()->string_format("%s\n", infoLog));
        delete[] infoLog;
    }
    else
        this->glDoLog(Settings::Instance()->string_format("Name %d is not a program\n", program));
}

void GLUtils::printShaderLog(GLuint shader) {
    if (glIsShader(shader)) {
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char* infoLog = new char[maxLength];
        
        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0)
            this->glDoLog(Settings::Instance()->string_format("%s\n", infoLog));
        delete[] infoLog;
    }
    else
        this->glDoLog(Settings::Instance()->string_format("Name %d is not a shader\n", shader));
}

bool GLUtils::logOpenGLError(const char *file, int line) {
    GLenum err = glGetError();
    bool success = true;
    
    while (err != GL_NO_ERROR) {
        std::string error;
        switch (err) {
            case GL_INVALID_OPERATION:
                error = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_ENUM:
                error = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "GL_INVALID_VALUE";
                break;
#if defined __gl_h_ || defined __gl3_h_
            case GL_OUT_OF_MEMORY:
                error = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
#endif
#if defined __gl_h_
            case GL_STACK_OVERFLOW:
                error = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "GL_STACK_UNDERFLOW";
                break;
            case GL_TABLE_TOO_LARGE:
                error = "GL_TABLE_TOO_LARGE";
                break;
#endif
            default:
                error = "(ERROR: Unknown Error Enum)";
                break;
        }
        success = false;
        this->glDoLog("Error occured at " + std::string(file) + " on line " + std::to_string(line) + " : " + error);
        err = glGetError();
    }
    
    return success;
}

#pragma mark - Utilities

void GLUtils::glDoLog(std::string logMessage) {
    if (this->doLog)
        this->doLog(logMessage);
    else
        printf("%s\n", logMessage.c_str());
}

std::string GLUtils::readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        this->glDoLog("Could not read file " + std::string(filePath) + ". File does not exist.");
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
