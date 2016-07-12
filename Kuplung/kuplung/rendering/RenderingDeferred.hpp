//
//  RenderingDeferred.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingDeferred_hpp
#define RenderingDeferred_hpp

#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"

class RenderingDeferred {
public:
    ~RenderingDeferred();
    void destroy();

    bool init();
    void render(std::vector<ModelFaceData*> meshModelFaces, ObjectsManager *managerObjects);

private:
    bool initGeometryPass();
    bool initLighingPass();
    bool initLightObjects();
    void initProps();
    void initGBuffer();
    void initModels(std::vector<ModelFaceBase*> meshModelFaces);
    void initLights();

    void renderGBuffer(std::vector<ModelFaceData*> meshModelFaces, ObjectsManager *managerObjects);
    void renderLightingPass(ObjectsManager *managerObjects);
    void renderLightObjects(ObjectsManager *managerObjects);

    glm::mat4 matrixProject, matrixCamera;

    GLUtils *glUtils;

    GLuint shaderProgram_GeometryPass, shaderProgram_LightingPass, shaderProgram_LightBox;
    GLint gl_GeometryPass_Texture_Diffuse, gl_GeometryPass_Texture_Specular;

    int GLSL_LightSourceNumber_Directional, GLSL_LightSourceNumber_Point, GLSL_LightSourceNumber_Spot;
    std::vector<ModelFace_LightSource_Directional *> mfLights_Directional;
    std::vector<ModelFace_LightSource_Point *> mfLights_Point;
    std::vector<ModelFace_LightSource_Spot *> mfLights_Spot;

    GLuint gBuffer, gPosition, gNormal, gAlbedoSpec;

    const GLuint NR_LIGHTS = 32;
    std::vector<glm::vec3> objectPositions;
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
