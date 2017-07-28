//
//  RenderingDeferred.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingDeferred_hpp
#define RenderingDeferred_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"

class RenderingDeferred {
public:
    explicit RenderingDeferred(ObjectsManager &managerObjects);
    ~RenderingDeferred();

    bool init();
    void render(const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);

    std::vector<glm::vec3> objectPositions;

private:
    bool initGeometryPass();
    bool initLighingPass();
    bool initLightObjects();
    bool initProps();
    bool initGBuffer();
    bool initLights();

    void renderGBuffer(const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel);
    void renderLightingPass();
    void renderLightObjects();

    glm::mat4 matrixProject, matrixCamera;

    ObjectsManager &managerObjects;

    GLuint shaderProgram_GeometryPass, shaderProgram_LightingPass, shaderProgram_LightBox;
    GLint gl_GeometryPass_Texture_Diffuse, gl_GeometryPass_Texture_Specular;

    unsigned int GLSL_LightSourceNumber_Directional, GLSL_LightSourceNumber_Point, GLSL_LightSourceNumber_Spot;
	std::vector<std::unique_ptr<ModelFace_LightSource_Directional>> mfLights_Directional;
	std::vector<std::unique_ptr<ModelFace_LightSource_Point>> mfLights_Point;
	std::vector<std::unique_ptr<ModelFace_LightSource_Spot>> mfLights_Spot;

    GLuint gBuffer, gPosition, gNormal, gAlbedoSpec;

    const GLuint NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    void renderQuad();

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;
    void renderCube();
};

#endif /* RenderingDeferred_hpp */
