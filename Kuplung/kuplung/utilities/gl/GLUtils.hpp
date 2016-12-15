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
#include "kuplung/utilities/gl/GLIncludes.h"

struct GridMeshPoint2D {
    GLfloat x, y;
};

struct GridMeshPoint3D {
    GLfloat x, y, z;
};

class GLUtils {
public:
    ~GLUtils();
    GLUtils(std::function<void(std::string)> logFunction);
    GLUtils();
    bool compileAndAttachShader(GLuint &shaderProgram, GLuint &shader, GLenum shaderType, const char *shader_source);
    bool compileShader(GLuint &shader, GLenum shaderType, const char *shader_source);
    std::string readFile(const char *filePath);

    void printProgramLog(GLuint program);
    void printShaderLog(GLuint shader);
    bool logOpenGLError(const char *file, int line);
    GLint glGetAttribute(GLuint program, const char* var_name);
    GLint glGetUniform(GLuint program, const char* var_name);
    GLint glGetAttributeNoWarning(GLuint program, const char* var_name);
    GLint glGetUniformNoWarning(GLuint program, const char* var_name);
    GLsizei getGLTypeSize(GLenum type);

private:
    std::function<void(std::string)> funcLog;
};

#endif /* GLUtils_hpp */
