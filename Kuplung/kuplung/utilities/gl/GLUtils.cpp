//
//  GLUtils.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "GLUtils.hpp"
#include "kuplung/settings/Settings.h"
#include <fstream>

namespace KuplungApp::Utilities::GL {

GLUtils::~GLUtils() {}

GLUtils::GLUtils(const std::function<void(std::string)>& logFunction) : funcLog(logFunction) {}

GLUtils::GLUtils() {}

bool GLUtils::compileAndAttachShader(GLuint& shaderProgram, GLuint& shader, GLenum shaderType, const char* shader_source) {
  shader = glCreateShader(shaderType);

  glShaderSource(shader, 1, &shader_source, nullptr);
  glCompileShader(shader);

  GLint isShaderCompiled = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);
  if (isShaderCompiled != GL_TRUE) {
    this->funcLog("Unable to compile shader " + std::to_string(shader) + "!");
    this->printShaderLog(shader);
    glDeleteShader(shader);
    return false;
  }
  else
    glAttachShader(shaderProgram, shader);
  return true;
}

bool GLUtils::compileShader(const GLuint& shaderProgram, GLenum shaderType, const char* shader_source) {
  const GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shader_source, nullptr);
  glCompileShader(shader);

  GLint isOK = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isOK);
  if (isOK != GL_TRUE) {
    this->funcLog(Settings::Instance()->string_format("Unable to compile shader ", shader, "!"));
    this->printShaderLog(shader);
    glDeleteShader(shader);
    return false;
  }

  glAttachShader(shaderProgram, shader);

  return true;
}

void GLUtils::CheckForGLErrors(const std::string& message) {
  if (Settings::Instance()->showGLErrors) {
    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      std::string errMessage = Settings::Instance()->string_format("[GLError] [", message, "] glError = ", error);
      if (std::find(this->reportedErrors.begin(), this->reportedErrors.end(), errMessage) == this->reportedErrors.end()) {
        Settings::Instance()->funcDoLog(errMessage);
        this->reportedErrors.push_back(errMessage);
      }
    }
  }
}

GLint GLUtils::glGetAttribute(GLuint program, const char* var_name, const std::source_location& location) const {
  const GLint var = glGetAttribLocation(program, var_name);
  if (var == -1)
    this->funcLog(Settings::Instance()->string_format("[GLUtils] Cannot fetch shader attribute ", var_name, " [", location.file_name(), ":", location.function_name(), ":", location.line(), "]!"));
  return var;
}

GLint GLUtils::glGetUniform(GLuint program, const char* var_name, const std::source_location& location) const {
  const GLint var = glGetUniformLocation(program, var_name);
  if (var == -1)
    this->funcLog(Settings::Instance()->string_format("[GLUtils] Cannot fetch shader uniform - ", var_name, " [", location.file_name(), ":", location.function_name(), ":", location.line(), "]!"));
  //this->CheckForGLErrors(Settings::Instance()->string_format("[GLUtils] Error at glGetUniform: ", var_name, " [", location.file_name(), ":", location.function_name(), ":", location.line(), "]!"));
  return var;
}

GLint GLUtils::glGetAttributeNoWarning(GLuint program, const char* var_name) {
  return glGetAttribLocation(program, var_name);
}

GLint GLUtils::glGetUniformNoWarning(GLuint program, const char* var_name) {
  return glGetUniformLocation(program, var_name);
}

void GLUtils::printProgramLog(GLuint program) {
  if (glIsProgram(program)) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    char* infoLog = new char[maxLength];
    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0)
      this->funcLog(infoLog);
    delete[] infoLog;
  }
  else
    this->funcLog("Name " + std::to_string(program) + " is not a program!");
}

void GLUtils::printShaderLog(GLuint shader) {
  if (glIsShader(shader)) {
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char* infoLog = new char[maxLength];

    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0)
      this->funcLog(infoLog);
    delete[] infoLog;
  }
  else
    this->funcLog("Name " + std::to_string(shader) + " is not a shader!");
}

bool GLUtils::logOpenGLError(const char* file, int line) {
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
    this->funcLog(Settings::Instance()->string_format("Error occured at ", file, " on line ", line, " : ", error));
    err = glGetError();
  }

  return success;
}

GLsizei GLUtils::getGLTypeSize(GLenum type) {
  switch (type) {
    case GL_BYTE:
      return sizeof(GLbyte);
    case GL_UNSIGNED_BYTE:
      return sizeof(GLubyte);
    case GL_SHORT:
      return sizeof(GLshort);
    case GL_UNSIGNED_SHORT:
      return sizeof(GLushort);
    case GL_INT:
      return sizeof(GLint);
    case GL_UNSIGNED_INT:
      // return sizeof(GLuint);
    case GL_FLOAT:
      return sizeof(GLfloat);
  }
  return 0;
}

std::string GLUtils::readFile(const char* filePath) {
  std::string content;
  std::ifstream fileStream(filePath, std::ios::in);
  if (!fileStream.is_open()) {
    this->funcLog(Settings::Instance()->string_format("Could not read file ", filePath, ". File does not exist."));
    return "";
  }
  std::string line("");
  while (!fileStream.eof()) {
    std::getline(fileStream, line);
    content.append(line + "\n");
  }
  fileStream.close();
  return content;
}

}
