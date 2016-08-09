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

void RenderingManager::render(std::unique_ptr<ObjectsManager> &managerObjects) {
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
    this->RenderingTotalVertices = 0;
    this->RenderingTotalIndices = 0;
    this->RenderingTotalTriangles = 0;
    this->RenderingTotalFaces = 0;
    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        ModelFaceBase* mmf = this->meshModelFaces[i];
        this->RenderingTotalVertices += mmf->meshModel.countVertices;
        if (mmf->meshModel.indices.size() > 0)
            this->RenderingTotalIndices += ((*std::max_element(mmf->meshModel.indices.begin(), mmf->meshModel.indices.end())) + 1);
        this->RenderingTotalTriangles += this->RenderingTotalVertices / 3;
        this->RenderingTotalFaces += this->RenderingTotalTriangles / 2;
    }

    Settings::Instance()->sceneCountObjects = int(this->meshModelFaces.size());
    Settings::Instance()->sceneCountVertices = this->RenderingTotalVertices;
    Settings::Instance()->sceneCountIndices = this->RenderingTotalIndices;
    Settings::Instance()->sceneCountTriangles = this->RenderingTotalTriangles;
    Settings::Instance()->sceneCountFaces = this->RenderingTotalFaces;
}

