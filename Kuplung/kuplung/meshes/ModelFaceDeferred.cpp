//
//  ModelFaceDeferred.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceDeferred.hpp"

void ModelFaceDeferred::initBuffers(std::string assetsFolder) {
    ModelFace::initBuffers(assetsFolder);
}

bool ModelFaceDeferred::initShaderProgram() {
    bool success = true;

    success |= this->gBuffer.init(Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    success |= this->geometryPass.init();

    this->geometryPass.enable();
    this->geometryPass.setColorTextureUnit(0);

    return success;
}

void ModelFaceDeferred::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
    this->matrixModel = matrixModel;
    this->vecCameraPosition = vecCameraPosition;
    this->grid = grid;
    this->uiAmbientLight = uiAmbientLight;

    this->doGeometryPass();
    this->doLightPass();
}

void ModelFaceDeferred::doGeometryPass() {
    this->geometryPass.enable();
    this->gBuffer.bindForWriting();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;
    this->geometryPass.setWVP(mvpMatrix);
    this->geometryPass.setWorldMatrix(this->matrixModel);

    glBindVertexArray(this->glVAO);

    //glDrawElementsBaseVertex(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, &this->meshModel.indices[0], 0);
    glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

void ModelFaceDeferred::doLightPass() {
    this->gBuffer.bindForReading();

    int w = Settings::Instance()->SDL_Window_Width;
    int h = Settings::Instance()->SDL_Window_Height;
    int HalfWidth = (int) (w / 2);
    int HalfHeight = (int) (h / 2);

    this->gBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(0, 0, w, h, 0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    this->gBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glBlitFramebuffer(0, 0, w, h, 0, HalfHeight, HalfWidth, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    this->gBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, w, h, HalfWidth, HalfHeight, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    this->gBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
    glBlitFramebuffer(0, 0, w, h, HalfWidth, 0, w, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
