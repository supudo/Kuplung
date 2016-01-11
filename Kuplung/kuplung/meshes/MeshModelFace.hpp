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
    void setOptionsDisplacement(glm::vec3 displacement);
    void setOptionsRefraction(float refraction);
    void setOptionsShininess(float shininess);
    void setOptionsStrengthSpecular(float val);
    void setOptionsStrengthAmbient(float val);
    void setOptionsStrengthDiffuse(float val);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition);
    objModelFace oFace;
    int ModelID;
    glm::mat4 matrixProjection, matrixCamera, matrixModel;
    glm::vec3 vecCameraPosition;

private:
    std::function<void(std::string)> doLogFunc;

    int glslVersion;
    std::string shaderName;

    float so_fov, so_alpha, so_refraction, so_shininess, so_strengthSpecular, so_strengthAmbient, so_strengthDiffuse;
    glm::vec3 so_lightPosition, so_lightDirection, so_lightColor, so_displacement;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment, shaderGeometry, shaderTessControl, shaderTessEval;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboTextureColor, vboIndices;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureShiness, vboTextureDissolve;

    GLuint glVS_MVPMatrix, glFS_MMatrix;
    GLuint glVS_VertexPosition, glFS_TextureCoord, glVS_VertexNormal, glFS_Sampler;
    GLuint glGS_GeomDisplacementLocation;
    GLuint glFS_AlphaBlending, glFS_CameraPosition, glFS_OpticalDensity, glFS_Shininess;
    GLuint glFS_StrengthSpecular, glFS_StrengthAmbient, glFS_StrengthDiffuse;
    GLuint glFS_Light_Position, glFS_Light_Direction;
    GLuint glFS_AmbientColor, glFS_DiffuseColor, glFS_SpecularColor;
    GLuint glFS_ScreenResX, glFS_ScreenResY;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* MeshModelFace_hpp */
