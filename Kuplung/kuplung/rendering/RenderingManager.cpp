//
//  RenderingManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingManager.hpp"

void RenderingManager::init() {
    this->renderingForward = new RenderingForward();
    this->renderingForward->init();

    this->renderingDeferred = new RenderingDeferred();
    this->renderingDeferred->init();
}

void RenderingManager::render(std::vector<ModelFaceBase*> meshModelFaces, glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight, int lightingPass_DrawMode) {
    if (Settings::Instance()->DeferredRendering)
        this->renderingDeferred->render(meshModelFaces, matrixProjection, matrixCamera, vecCameraPosition, grid, uiAmbientLight, lightingPass_DrawMode);
    else
        this->renderingForward->render(meshModelFaces, matrixProjection, matrixCamera, vecCameraPosition, grid, uiAmbientLight, lightingPass_DrawMode);
}

