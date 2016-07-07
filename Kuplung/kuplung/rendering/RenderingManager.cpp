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

    this->RenderingTotalVertices = 0;
    this->RenderingTotalIndices = 0;
    this->RenderingTotalTriangles = 0;
    this->RenderingTotalFaces = 0;
}

void RenderingManager::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight, int lightingPass_DrawMode) {
    if (Settings::Instance()->DeferredRendering)
        this->renderingDeferred->render(this->meshModelFaces, matrixProjection, matrixCamera, vecCameraPosition, grid, uiAmbientLight, lightingPass_DrawMode);
    else
        this->renderingForward->render(this->meshModelFaces, matrixProjection, matrixCamera, vecCameraPosition, grid, uiAmbientLight, lightingPass_DrawMode);
}

