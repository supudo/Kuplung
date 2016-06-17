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
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/deferred/GBuffer.hpp"
#include "kuplung/meshes/deferred/GeometryPass.hpp"

class ModelFaceDeferred: public ModelFace {
private:
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight);
    void doGeometryPass();
    void doLightPass();

    GBuffer gBuffer;
    GeometryPass geometryPass;
};

#endif /* ModelFaceDeferred_hpp */
