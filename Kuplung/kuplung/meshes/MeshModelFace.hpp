//
//  MeshModelFace.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MeshModelFace_hpp
#define MeshModelFace_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "utilities/gl/GLIncludes.h"
#include "utilities/parsers/obj-parser/objObjects.h"
#include "utilities/gl/GLUtils.hpp"

class MeshModelFace {
public:
    ~MeshModelFace();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    void setModel(objModelFace oFace);
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void setOptionsFOV(float fov);
    void setOptionsAlpha(float alpha);
    void setOptionsLightPosition(glm::vec3 lightPosition);
    void setOptionsLightDirection(glm::vec3 lightDirection);
    void setOptionsLightColor(glm::vec3 lightColor);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition);
    objModelFace oFace;
    int ModelID;
    glm::mat4 matrixProjection, matrixCamera, matrixModel;
    glm::vec3 vecCameraPosition;

private:
    std::function<void(std::string)> doLogFunc;

    bool compileShader(GLuint &shader, GLenum shaderType, const char *shader_source);

    int glslVersion;
    std::string shaderName;

    float so_fov, so_alpha;
    glm::vec3 so_lightPosition, so_lightDirection, so_lightColor;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment, shaderGeometry, shaderTessControl, shaderTessEval;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboTextureColor, vboIndices;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureShiness, vboTextureDissolve;
    GLuint glUniformMVPMatrix, glUniformMMatrix;
    GLuint glAttributeVertexPosition, glAttributeTextureCoord, glAttributeVertexNormal, glUniformSampler;
    GLuint glUniformAlphaBlending, glUniform_CameraPosition;
    GLuint glUniformLight_Position, glUniformLight_Direction;
    GLuint glUniform_ambientColor, glUniform_diffuseColor, glUniform_specularColor;
    GLuint glGeomDisplacementLocation;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* MeshModelFace_hpp */
