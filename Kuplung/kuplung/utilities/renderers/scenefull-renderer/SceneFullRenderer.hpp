//
//  SceneFullRenderer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SceneFullRenderer_hpp
#define SceneFullRenderer_hpp

#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/renderers/KuplungRendererBase.hpp"

class SceneFullRenderer: public KuplungRendererBase {
public:
    SceneFullRenderer(ObjectsManager &managerObjects);
    ~SceneFullRenderer();
    void init();
    std::string renderImage(FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces);

private:
    FBEntity fileOutputImage;

    bool initGeometryPass();
    bool initLighingPass();
    void initGBuffer();
    void initModels(std::vector<ModelFaceBase*> meshModelFaces);
    void initLights();

    void renderGBuffer(std::vector<ModelFaceBase*> *meshModelFaces);
    void renderLightingPass();

    glm::mat4 matrixProject, matrixCamera;

    std::unique_ptr<GLUtils> glUtils;
    ObjectsManager &managerObjects;

    GLuint shaderProgram_GeometryPass, shaderProgram_LightingPass;
    GLint gl_GeometryPass_Texture_Diffuse, gl_GeometryPass_Texture_Specular;

    unsigned int GLSL_LightSourceNumber_Directional, GLSL_LightSourceNumber_Point, GLSL_LightSourceNumber_Spot;
    std::vector<ModelFace_LightSource_Directional *> mfLights_Directional;
    std::vector<ModelFace_LightSource_Point *> mfLights_Point;
    std::vector<ModelFace_LightSource_Spot *> mfLights_Spot;

    GLuint gBuffer, gPosition, gNormal, gAlbedoSpec;

    const GLuint NR_LIGHTS = 32;

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    void renderQuad();
};

#endif /* SceneFullRenderer_hpp */
