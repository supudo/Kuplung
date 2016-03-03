//
//  ModelFace.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFace_hpp
#define ModelFace_hpp

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/ui/Objects.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/Light.hpp"
#include "kuplung/meshes/WorldGrid.hpp"
#include "kuplung/utilities/maths/Maths.hpp"

struct ModelFace_LightSource {
    GLuint glLight_InUse;
    GLuint glLight_Ambient, glLight_Diffuse, glLight_Specular;
    GLuint glLight_StrengthAmbient, glLight_StrengthDiffuse, glLight_StrengthSpecular;
    GLuint glLight_Position, glLight_Direction;
};

class ModelFace {
public:
    ModelFace();
    ~ModelFace();
    ModelFace* clone(int modelID);

    void destroy();
    void init(std::function<void(std::string)> doLog);
    void setModel(objModelFace oFace);
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void initProperties();
    void initModelProperties();
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid);

    // general options
    void setOptionsFOV(float fov);

    // outlining
    void setOptionsSelected(bool selectedYn);
    void setOptionsOutlineColor(glm::vec4 outlineColor);
    void setOptionsOutlineThickness(float thickness);

    bool glUseTessellation;
    objModelFace oFace;
    int ModelID;
    glm::mat4 matrixProjection, matrixCamera, matrixModel;
    glm::vec3 vecCameraPosition;
    std::vector<Light*> lightSources;
    std::vector<GLfloat> dataVertices;
    std::vector<GLfloat> dataTexCoords;
    std::vector<GLfloat> dataNormals;
    std::vector<GLuint> dataIndices;

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
    bool showMaterialEditor;
    MaterialColor *materialAmbient, *materialDiffuse, *materialSpecular, *materialEmission;

private:
    std::function<void(std::string)> doLogFunc;
    void drawOutline();
    void drawOnly();

    bool reflectionInit();
    void renderModel();
    void renderReflectFBO();
    void renderMirrorSurface();
    void loadTexture(std::string assetsFolder, objMaterialImage materialImage, objMaterialImageType type, GLuint* vboObject);

    float so_fov;
    float so_outlineThickness;
    glm::vec4 so_outlineColor;
    bool so_selectedYn;
    WorldGrid *grid;

    GLUtils *glUtils;
    Maths *mathHelper;

    // model objects
    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment, shaderGeometry, shaderTessControl, shaderTessEval;
    GLuint fboDefault, glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve;
    GLuint vboTextureBump;

    // reflection objects
    GLuint shaderProgramReflection;
    GLuint shaderVertexReflection, shaderFragmentReflection;
    GLuint fboReflection, glVAOReflect;
    GLuint vboVerticesReflect, vboNormalsReflect, vboTextureCoordinatesReflect, vboIndicesReflect;
    GLuint reflectTexName, reflectWidth, reflectHeight;
    GLuint reflectModelViewUniformIdx, reflectProjectionUniformIdx, reflectNormalMatrixUniformIdx;

    // variables
    GLuint glVS_MVPMatrix, glFS_MMatrix, glVS_WorldMatrix;
    GLuint glVS_VertexPosition, glFS_TextureCoord, glVS_VertexNormal, glVS_Tangent;

    // general
    GLuint glGS_GeomDisplacementLocation;
    GLuint glFS_AlphaBlending, glFS_CameraPosition, glFS_CelShading;
    GLuint glFS_OutlineColor, glVS_IsBorder;
    GLuint glFS_ScreenResX, glFS_ScreenResY;

    // light
    int GLSL_LightSource_Number;
    std::vector<ModelFace_LightSource *> mfLights;

    // material
    GLuint glMaterial_Ambient, glMaterial_Diffuse, glMaterial_Specular, glMaterial_SpecularExp;
    GLuint glMaterial_Emission, glMaterial_Refraction, glMaterial_IlluminationModel;
    GLuint glMaterial_SamplerAmbient, glMaterial_SamplerDiffuse, glMaterial_SamplerSpecular;
    GLuint glMaterial_SamplerSpecularExp, glMaterial_SamplerDissolve, glMaterial_SamplerBump;
    GLuint glMaterial_HasTextureAmbient, glMaterial_HasTextureDiffuse, glMaterial_HasTextureSpecular;
    GLuint glMaterial_HasTextureSpecularExp, glMaterial_HasTextureDissolve, glMaterial_HasTextureBump;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* ModelFace_hpp */
