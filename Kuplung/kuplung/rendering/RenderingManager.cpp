//
//  RenderingManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingManager.hpp"

void RenderingManager::init() {
    this->rendererSimple = new RenderingSimple();
    this->rendererSimple->init();

    this->rendererForward = new RenderingForward();
    this->rendererForward->init();

    this->rendererDeferred = new RenderingDeferred();
    this->rendererDeferred->init();

    this->RenderingTotalVertices = 0;
    this->RenderingTotalIndices = 0;
    this->RenderingTotalTriangles = 0;
    this->RenderingTotalFaces = 0;
}

void RenderingManager::render(ObjectsManager *managerObjects) {
    switch (Settings::Instance()->RendererType) {
        case 0:
            this->rendererSimple->render(this->meshModelFaces, managerObjects);
            break;
        case 1:
            this->rendererForward->render(this->meshModelFaces, managerObjects);
            break;
        case 2:
            this->rendererDeferred->render(this->meshModelFaces, managerObjects);
            break;
        default:
            break;
    }
}

