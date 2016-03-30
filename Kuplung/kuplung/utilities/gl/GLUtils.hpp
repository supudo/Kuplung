//
//  GLUtils.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GLUtils_hpp
#define GLUtils_hpp

#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/settings/Settings.h"

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
    GLuint initShaderProgram(std::string shaderVertexName, std::string shaderFragmentName);
    bool compileShader(GLuint &shaderProgram, GLuint &shader, GLenum shaderType, const char *shader_source);

    void printProgramLog(GLuint program);
    void printShaderLog(GLuint shader);
    bool logOpenGLError(const char *file, int line);
    GLint glGetAttribute(GLuint program, const char* var_name);
    GLint glGetUniform(GLuint program, const char* var_name);
    GLsizei getGLTypeSize(GLenum type);

private:
    std::string readFile(const char *filePath);
};

#endif /* GLUtils_hpp */
