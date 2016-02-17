//
//  ModelFace.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFace_hpp
#define ModelFace_hpp

#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/ui/Objects.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/Light.hpp"

class ModelFace {
public:
    ModelFace();
    ~ModelFace();
    ModelFace* clone(int modelID);

    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    void setModel(objModelFace oFace);
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void initProperties();
    void initModelProperties();
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, bool fixedWithGrid, glm::mat4 matrixGrid);

    // general options
    void setOptionsFOV(float fov);

    // outlining
    void setOptionsSelected(bool selectedYn);
    void setOptionsOutlineColor(glm::vec4 outlineColor);
    void setOptionsOutlineThickness(float thickness);

    objModelFace oFace;
    int ModelID;
    glm::mat4 matrixProjection, matrixCamera, matrixModel;
    glm::vec3 vecCameraPosition;
    std::vector<Light*> lightSources;

    bool Setting_CelShading;
    float Setting_Alpha;
    ObjectEye *Settings_Eye;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;
    ObjectCoordinate *displaceX, *displaceY, *displaceZ;
    ObjectCoordinate *Setting_MaterialRefraction;
    ObjectCoordinate *Setting_MaterialSpecularExp;

    // light
    glm::vec3 Setting_LightPosition, Setting_LightDirection;
    glm::vec3 Setting_LightAmbient, Setting_LightDiffuse, Setting_LightSpecular;
    float Setting_LightStrengthAmbient, Setting_LightStrengthDiffuse, Setting_LightStrengthSpecular;

    // material
    int materialIlluminationModel;
    MaterialColor *materialAmbient, *materialDiffuse, *materialSpecular, *materialEmission;

private:
    std::function<void(std::string)> doLogFunc;
    void drawOnly();
    void outlineThree();
    void outlineTwo();
    void outlineOne();

    int glslVersion;
    std::string shaderName;

    // general
    float so_fov;

    // outline
    float so_outlineThickness;
    glm::vec4 so_outlineColor;

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

#endif /* ModelFace_hpp */