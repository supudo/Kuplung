//
//  GLUtils.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GLUtils_hpp
#define GLUtils_hpp

#include <string>
#include <functional>
#include <vector>
#include "kuplung/utilities/gl/GLIncludes.h"
#include <source_location>

namespace KuplungApp::Utilities::GL {

class GLUtils {
public:
  ~GLUtils();
  explicit GLUtils(const std::function<void(std::string)>& logFunction);
  GLUtils();
  bool compileAndAttachShader(GLuint &shaderProgram, GLuint &shader, GLenum shaderType, const char *shader_source);
  bool compileShader(const GLuint &shader, GLenum shaderType, const char *shader_source);
  std::string readFile(const char *filePath);
  void CheckForGLErrors(const std::source_location& location = std::source_location::current());

  void printProgramLog(GLuint program);
  void printShaderLog(GLuint shader);
  bool logOpenGLError(const char *file, int line);
  GLint glGetAttribute(GLuint program, const char* var_name, const std::source_location& location = std::source_location::current()) const;
  GLint glGetUniform(GLuint program, const char* var_name, const std::source_location& location = std::source_location::current()) const;
  GLint glGetAttributeNoWarning(GLuint program, const char* var_name);
  GLint glGetUniformNoWarning(GLuint program, const char* var_name);
  GLsizei getGLTypeSize(GLenum type);

private:
  std::function<void(std::string)> funcLog;
  std::vector<std::string> reportedErrors;
};

}

#endif /* GLUtils_hpp */
