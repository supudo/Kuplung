//
//  MeshModelFace.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MeshModelFace_hpp
#define MeshModelFace_hpp

#include <glm/gtc/matrix_transform.hpp>
#include <functional>
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
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition);

    // general options
    void setOptionsFOV(float fov);
    void setOptionsAlpha(float alpha);
    void setOptionsDisplacement(glm::vec3 displacement);
    void setOptionsCelShading(bool val);

    // light
    void setOptionsLightPosition(glm::vec3 lightPosition);
    void setOptionsLightDirection(glm::vec3 lightDirection);
    void setOptionsLightAmbient(glm::vec3 lightColor);
    void setOptionsLightDiffuse(glm::vec3 lightColor);
    void setOptionsLightSpecular(glm::vec3 lightColor);
    void setOptionsLightStrengthAmbient(float val);
    void setOptionsLightStrengthDiffuse(float val);
    void setOptionsLightStrengthSpecular(float val);

    // material
    void setOptionsMaterialRefraction(float val);
    void setOptionsMaterialSpecularExp(float val);
    void setOptionsMaterialIlluminationModel(float val);
    void setOptionsMaterialAmbient(glm::vec3 lightColor);
    void setOptionsMaterialDiffuse(glm::vec3 lightColor);
    void setOptionsMaterialSpecular(glm::vec3 lightColor);
    void setOptionsMaterialEmission(glm::vec3 lightColor);

    // outlining
    void setOptionsSelected(bool selectedYn);
    void setOptionsOutlineColor(glm::vec4 outlineColor);
    void setOptionsOutlineThickness(float thickness);

    objModelFace oFace;
    int ModelID, ModelIndex;
    glm::mat4 matrixProjection, matrixCamera, matrixModel;
    glm::vec3 vecCameraPosition;

private:
    std::function<void(std::string)> doLogFunc;
    void drawOnly();
    void outlineThree();
    void outlineTwo();
    void outlineOne();

    int glslVersion;
    std::string shaderName;

    // general
    float so_fov, so_alpha;
    bool so_celShading;
    glm::vec3 so_displacement;

    // outline
    float so_outlineThickness;
    glm::vec4 so_outlineColor;

    // light
    glm::vec3 so_lightPosition, so_lightDirection;
    glm::vec3 so_lightAmbient, so_lightDiffuse, so_lightSpecular;
    float so_lightStrengthAmbient, so_lightStrengthDiffuse, so_lightStrengthSpecular;

    // material
    float so_materialRefraction, so_materialSpecularExp, so_materialIlluminationModel;
    glm::vec3 so_materialAmbient, so_materialDiffuse, so_materialSpecular, so_materialEmission;

    bool so_selectedYn;

    GLUtils *glUtils;

    // program, shaders, VAO, VBO
    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment, shaderGeometry, shaderTessControl, shaderTessEval;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve;

    // variables
    GLuint glVS_MVPMatrix, glFS_MMatrix;
    GLuint glVS_VertexPosition, glFS_TextureCoord, glVS_VertexNormal;

    // general
    GLuint glGS_GeomDisplacementLocation;
    GLuint glFS_AlphaBlending, glFS_CameraPosition, glFS_CelShading;
    GLuint glFS_OutlineColor, glVS_IsBorder;
    GLuint glFS_ScreenResX, glFS_ScreenResY;

    // light
    GLuint glLight_Ambient, glLight_Diffuse, glLight_Specular;
    GLuint glLight_StrengthAmbient, glLight_StrengthDiffuse, glLight_StrengthSpecular;
    GLuint glLight_Position, glLight_Direction;

    // material
    GLuint glMaterial_Ambient, glMaterial_Diffuse, glMaterial_Specular, glMaterial_SpecularExp;
    GLuint glMaterial_Emission, glMaterial_Refraction, glMaterial_IlluminationModel;
    GLuint glMaterial_SamplerAmbient, glMaterial_SamplerDiffuse, glMaterial_SamplerSpecular;
    GLuint glMaterial_SamplerSpecularExp, glMaterial_SamplerDissolve;
    GLuint glMaterial_HasTextureAmbient, glMaterial_HasTextureDiffuse, glMaterial_HasTextureSpecular;
    GLuint glMaterial_HasTextureSpecularExp, glMaterial_HasTextureDissolve;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* MeshModelFace_hpp */
