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
    RenderingSimple();
    ~RenderingSimple();
    void destroy();

    bool init();
    void render(std::vector<ModelFaceData*> meshModelFaces, ObjectsManager *managerObjects);

private:
    GLUtils *glUtils;

    glm::mat4 matrixProjection, matrixCamera;
    glm::vec3 vecCameraPosition, uiAmbientLight;
    WorldGrid *grid;

    GLuint shaderProgram;

    GLuint glVS_VertexPosition, glFS_TextureCoord, glVS_VertexNormal, glVS_Tangent, glVS_Bitangent;
    GLuint glVS_MVPMatrix, glVS_WorldMatrix, glFS_SamplerTexture, glFS_HasSamplerTexture;

    GLuint glFS_CameraPosition, glFS_UIAmbient, glFS_solidSkin_materialColor;
    ModelFace_LightSource_Directional *solidLight;
};

#endif /* RenderingSimple_hpp */