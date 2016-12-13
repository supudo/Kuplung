//
//  RenderingSimple.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingSimple_hpp
#define RenderingSimple_hpp

#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"

class RenderingSimple {
public:
    explicit RenderingSimple(ObjectsManager &managerObjects);
    explicit RenderingSimple(std::unique_ptr<ObjectsManager> &managerObjects);
    ~RenderingSimple();

    bool init();
    void render(std::vector<ModelFaceData*> meshModelFaces, const int selectedModel);

private:
    std::unique_ptr<GLUtils> glUtils;
    ObjectsManager &managerObjects;

    glm::mat4 matrixProjection, matrixCamera;
    glm::vec3 vecCameraPosition, uiAmbientLight;

    GLuint shaderProgram;
    GLint glVS_MVPMatrix, glVS_WorldMatrix, glFS_SamplerTexture, glFS_HasSamplerTexture;
    GLint glFS_CameraPosition, glFS_UIAmbient;
    std::unique_ptr<ModelFace_LightSource_Directional> solidLight;
};

#endif /* RenderingSimple_hpp */
