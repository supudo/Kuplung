//
//  ModelFaceDeferred.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ModelFaceDeferred_hpp
#define ModelFaceDeferred_hpp

#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/meshes/deferred/GBuffer.hpp"
#include "kuplung/meshes/deferred/GeometryPass.hpp"

#define COLOR_TEXTURE_UNIT              GL_TEXTURE0
#define COLOR_TEXTURE_UNIT_INDEX        0
#define SHADOW_TEXTURE_UNIT             GL_TEXTURE1
#define SHADOW_TEXTURE_UNIT_INDEX       1
#define NORMAL_TEXTURE_UNIT             GL_TEXTURE2
#define NORMAL_TEXTURE_UNIT_INDEX       2
#define RANDOM_TEXTURE_UNIT             GL_TEXTURE3
#define RANDOM_TEXTURE_UNIT_INDEX       3
#define DISPLACEMENT_TEXTURE_UNIT       GL_TEXTURE4
#define DISPLACEMENT_TEXTURE_UNIT_INDEX 4
#define MOTION_TEXTURE_UNIT             GL_TEXTURE5
#define MOTION_TEXTURE_UNIT_INDEX       5

class ModelFaceDeferred: public ModelFace {
private:
    virtual bool initShaderProgram();
    virtual void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);

    void doGeometryPass();
    void doDirectionalLightPass(glm::vec3 vecCameraPosition);
    void doStencilPass(int pointLightIndex);
    void doFinalPass();

    GBuffer gBuffer;
    GeometryPass geometryPass;
};

#endif /* ModelFaceDeferred_hpp */
