//
//  GLUtils.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GLUtils_hpp
#define GLUtils_hpp

#include <functional>
#include "utilities/gl/GLIncludes.h"
#include "utilities/settings/Settings.h"

struct GridMeshPoint2D
{
    GLfloat x;
    GLfloat y;
};

struct GridMeshPoint3D
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

class GLUtils {
public:
    ~GLUtils();
    void init(std::function<void(std::string)> doLog);
    GLuint initShaderProgram(std::string shaderVertexName, std::string shaderFragmentName, int glslVersion);
    bool compileShader(GLuint &shaderProgram, GLuint &shader, GLenum shaderType, const char *shader_source);

    void printProgramLog(GLuint program);
    void printShaderLog(GLuint shader);
    bool logOpenGLError(const char *file, int line);
    GLint glGetAttribute(GLuint program, const char* var_name);
    GLint glGetUniform(GLuint program, const char* var_name);

private:
    std::function<void(std::string)> doLog;
    std::string readFile(const char *filePath);

    void glDoLog(std::string logMessage);
};

#endif /* GLUtils_hpp */
