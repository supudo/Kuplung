//
//  ModelFaceBase.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceBase_hpp
#define ModelFaceBase_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/ui/Objects.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/helpers/Light.hpp"
#include "kuplung/meshes/helpers/WorldGrid.hpp"
#include "kuplung/utilities/maths/Maths.hpp"
#include "kuplung/meshes/helpers/BoundingBox.hpp"

class ModelFaceBase {
public:
    ModelFaceBase();
    virtual ~ModelFaceBase();
    virtual void destroy();

    ModelFaceBase* clone(int modelID);

    virtual void init(MeshModel model, std::string assetsFolder);
    void initModelProperties();
    void initBoundingBox();
    virtual void initBuffers();
    void loadTexture(std::string assetsFolder, MeshMaterialTextureImage materialImage, objMaterialImageType type, GLuint* vboObject);

    virtual void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);

    // general options
    void setOptionsFOV(float fov);

    // outlining
    void setOptionsSelected(bool selectedYn);
    void setOptionsOutlineColor(glm::vec4 outlineColor);
    void setOptionsOutlineThickness(float thickness);
    bool getOptionsSelected();
    glm::vec4 getOptionsOutlineColor();
    float getOptionsOutlineThickness();

    BoundingBox *boundingBox;
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
    std::string assetsFolder;

    bool Settings_DeferredRender;
    bool Setting_CelShading, Setting_Wireframe, Setting_UseTessellation, Setting_UseCullFace;
    float Setting_Alpha;
    int Setting_TessellationSubdivision;
    std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
    std::unique_ptr<ObjectCoordinate> scaleX, scaleY, scaleZ;
    std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
    std::unique_ptr<ObjectCoordinate> displaceX, displaceY, displaceZ;
    std::unique_ptr<ObjectCoordinate> Setting_MaterialRefraction;
    std::unique_ptr<ObjectCoordinate> Setting_MaterialSpecularExp;

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
    std::unique_ptr<ObjectCoordinate> displacementHeightScale;
    bool showMaterialEditor;
    std::unique_ptr<MaterialColor> materialAmbient, materialDiffuse, materialSpecular, materialEmission;

    // mapping
    bool Setting_ParallaxMapping;

    // effects - gaussian blur
    int Effect_GBlur_Mode;
    std::unique_ptr<ObjectCoordinate> Effect_GBlur_Radius, Effect_GBlur_Width;

    // effects - bloom
    bool Effect_Bloom_doBloom;
    float Effect_Bloom_WeightA, Effect_Bloom_WeightB, Effect_Bloom_WeightC, Effect_Bloom_WeightD, Effect_Bloom_Vignette, Effect_Bloom_VignetteAtt;

    // rendering
    int Setting_LightingPass_DrawMode;

protected:
    float so_fov;
    float so_outlineThickness;
    glm::vec4 so_outlineColor;
    bool so_selectedYn;
    glm::vec3 uiAmbientLight;
    WorldGrid *grid;

    std::unique_ptr<GLUtils> glUtils;
    std::unique_ptr<Maths> mathHelper;
};

#endif /* ModelFaceBase_hpp */
