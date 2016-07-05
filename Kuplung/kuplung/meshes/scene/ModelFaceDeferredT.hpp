//
//  ModelFaceDeferredT.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceDeferredT_hpp
#define ModelFaceDeferredT_hpp

#include "kuplung/meshes/scene/ModelFace.hpp"
#include "kuplung/utilities/gl/GLUtils.hpp"

class ModelFaceDeferredT: public ModelFace {
public:
    void destroy();

    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);

private:
    GLuint shaderProgram, glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices, vboTangents, vboBitangents;
    GLuint glVS_VertexPosition, glVS_VertexNormal, glFS_TextureCoord, glVS_Tangent, glVS_Bitangent;
};

#endif /* ModelFaceDeferredT_hpp */
