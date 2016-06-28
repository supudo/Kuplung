//
//  ModelFace.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFace_hpp
#define ModelFace_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/ui/Objects.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/helpers/Light.hpp"
#include "kuplung/meshes/helpers/BoundingBox.hpp"
#include "kuplung/meshes/helpers/WorldGrid.hpp"
#include "kuplung/utilities/maths/Maths.hpp"

class ModelFace {
public:
    ModelFace();
    ~ModelFace();
    ModelFace* clone(int modelID);

    void destroy();
    void init();
    void setModel(MeshModel meshModel);
    virtual bool initShaderProgram();
    virtual void initBuffers(std::string assetsFolder);
    void initBoundingBox();
    void initProperties();
    void initModelProperties();
    virtual void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);
    void renderModel();

    // general options
    void setOptionsFOV(float fov);

    // outlining
    void setOptionsSelected(bool selectedYn);
    void setOptionsOutlineColor(glm::vec4 outlineColor);
    void setOptionsOutlineThickness(float thickness);

    bool initBuffersAgain;
    MeshModel meshModel;
    int ModelID;
    glm::mat4 matrixProjection, matrixCamera, matrixModel;
    glm::vec3 vecCameraPosition;
    std::vector<Light*> lightSources;
    std::vector<GLfloat> dataVertices;
    std::vector<GLfloat> dataTexCoords;
    std::vector<GLfloat> dataNormals;
    std::vector<GLuint> dataIndices;
    BoundingBox *boundingBox;
    std::string assetsFolder;

    bool Setting_CelShading, Setting_Wireframe, Setting_UseTessellation, Setting_UseCullFace;
    float Setting_Alpha;
    int Setting_TessellationSubdivision;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;
    ObjectCoordinate *displaceX, *displaceY, *displaceZ;
    ObjectCoordinate *Setting_MaterialRefraction;
    ObjectCoordinate *Setting_MaterialSpecularExp;

    // view skin
    ViewModelSkin Setting_ModelViewSkin;
    glm::vec3 solidLightSkin_MaterialColor, solidLightSkin_Ambient, solidLightSkin_Diffuse, solidLightSkin_Specular;
    float solidLightSkin_Ambient_Strength, solidLightSkin_Diffuse_Strength, solidLightSkin_Specular_Strength;

    // light
    glm::vec3 Setting_LightPosition, Setting_LightDirection;
    glm::vec3 Setting_LightAmbient, Setting_LightDiffuse, Setting_LightSpecular;
    float Setting_LightStrengthAmbient, Setting_LightStrengthDiffuse, Setting_LightStrengthSpecular;

    // material
    int materialIlluminationModel;
    ObjectCoordinate* displacementHeightScale;
    bool showMaterialEditor;
    MaterialColor *materialAmbient, *materialDiffuse, *materialSpecular, *materialEmission;

    // mapping
    bool Setting_ParallaxMapping;

    // effects - gaussian blur
    int Effect_GBlur_Mode;
    ObjectCoordinate *Effect_GBlur_Radius, *Effect_GBlur_Width;

    // effects - bloom
    bool Effect_Bloom_doBloom;
    float Effect_Bloom_WeightA, Effect_Bloom_WeightB, Effect_Bloom_WeightC, Effect_Bloom_WeightD, Effect_Bloom_Vignette, Effect_Bloom_VignetteAtt;

    // rendering
    int Setting_LightingPass_DrawMode;

protected:
    void drawOutline();
    void drawOnly();

    bool reflectionInit();
    void renderReflectFBO();
    void renderMirrorSurface();
    void loadTexture(std::string assetsFolder, MeshMaterialTextureImage materialImage, objMaterialImageType type, GLuint* vboObject);

    float so_fov;
    float so_outlineThickness;
    glm::vec4 so_outlineColor;
    bool so_selectedYn;
    glm::vec3 uiAmbientLight;
    WorldGrid *grid;

    GLUtils *glUtils;
    Maths *mathHelper;

    // model objects
    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment, shaderGeometry, shaderTessControl, shaderTessEval;
    GLuint fboDefault, glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;
    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureSpecular, vboTextureSpecularExp, vboTextureDissolve;
    GLuint vboTextureBump, vboTextureDisplacement;

    // reflection objects
    GLuint shaderProgramReflection;
    GLuint shaderVertexReflection, shaderFragmentReflection;
    GLuint fboReflection, glVAOReflect;
    GLuint vboVerticesReflect, vboNormalsReflect, vboTextureCoordinatesReflect, vboIndicesReflect;
    GLuint reflectTexName, reflectWidth, reflectHeight;
    GLuint reflectModelViewUniformIdx, reflectProjectionUniformIdx, reflectNormalMatrixUniformIdx;

    // variables
    GLint glVS_MVPMatrix, glFS_MMatrix, glVS_WorldMatrix, glVS_NormalMatrix, glFS_MVMatrix;
    GLuint glVS_VertexPosition, glFS_TextureCoord, glVS_VertexNormal, glVS_Tangent, glVS_Bitangent;

    // general
    GLint glGS_GeomDisplacementLocation, glFS_AlphaBlending, glFS_CameraPosition, glFS_CelShading;
    GLint glFS_OutlineColor, glVS_IsBorder, glFS_ScreenResX, glFS_ScreenResY, glFS_UIAmbient;
    GLint glTCS_UseCullFace, glTCS_UseTessellation, glTCS_TessellationSubdivision, gl_ModelViewSkin;

    // view skin
    ModelFace_LightSource_Directional *solidLight;
    GLint glFS_solidSkin_materialColor;

    // light
    int GLSL_LightSourceNumber_Directional, GLSL_LightSourceNumber_Point, GLSL_LightSourceNumber_Spot;
    std::vector<ModelFace_LightSource_Directional *> mfLights_Directional;
    std::vector<ModelFace_LightSource_Point *> mfLights_Point;
    std::vector<ModelFace_LightSource_Spot *> mfLights_Spot;

    // material
    GLint glMaterial_Ambient, glMaterial_Diffuse, glMaterial_Specular, glMaterial_SpecularExp;
    GLint glMaterial_Emission, glMaterial_Refraction, glMaterial_IlluminationModel, glMaterial_HeightScale;
    GLint glMaterial_SamplerAmbient, glMaterial_SamplerDiffuse, glMaterial_SamplerSpecular;
    GLint glMaterial_SamplerSpecularExp, glMaterial_SamplerDissolve, glMaterial_SamplerBump, glMaterial_SamplerDisplacement;
    GLint glMaterial_HasTextureAmbient, glMaterial_HasTextureDiffuse, glMaterial_HasTextureSpecular;
    GLint glMaterial_HasTextureSpecularExp, glMaterial_HasTextureDissolve, glMaterial_HasTextureBump, glMaterial_HasTextureDisplacement;
    GLint glMaterial_ParallaxMapping;

    // effects - gaussian blur
    GLint glEffect_GB_W, glEffect_GB_Radius, glEffect_GB_Mode;
    // effects - bloom
    GLint glEffect_Bloom_doBloom, glEffect_Bloom_WeightA, glEffect_Bloom_WeightB, glEffect_Bloom_WeightC, glEffect_Bloom_WeightD, glEffect_Bloom_Vignette, glEffect_Bloom_VignetteAtt;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* ModelFace_hpp */