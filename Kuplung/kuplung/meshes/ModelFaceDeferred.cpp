//
//  ModelFaceDeferred.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceDeferred.hpp"

bool ModelFaceDeferred::initShaderProgram() {
    bool success = true;

    success |= ModelFace::initShaderProgram();
    if (!success)
        Settings::Instance()->funcDoLog("[Deferred] Cannot initialize program!");

    success |= this->gBuffer.init();
    if (!success)
        Settings::Instance()->funcDoLog("[Deferred] Cannot initialize GBuffer!");

    success |= this->geometryPass.init();
    if (!success)
        Settings::Instance()->funcDoLog("[Deferred] Cannot initialize Geometry Pass!");
    this->geometryPass.enable();
    this->geometryPass.setColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);

    return success;
}

void ModelFaceDeferred::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight) {
    this->gBuffer.startFrame();
    this->doGeometryPass();
//    glEnable(GL_STENCIL_TEST);

    ModelFace::render(matrixProjection, matrixCamera, matrixModel, vecCameraPosition, grid, uiAmbientLight);

//    glDisable(GL_STENCIL_TEST);
    this->doFinalPass();
}

void ModelFaceDeferred::doGeometryPass() {
    this->geometryPass.enable();
    this->gBuffer.bindForGeomPass();

    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

//    Pipeline p;
//    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
//    p.SetPerspectiveProj(m_persProjInfo);
//    p.Rotate(0.0f, m_scale, 0.0f);

//    for (size_t i=0 ; i<this->boxPositions.size(); i++) {
//        p.WorldPos(this->boxPositions[i]);
//        m_DSGeomPassTech.SetWVP(p.GetWVPTrans());
//        m_DSGeomPassTech.SetWorldMatrix(p.GetWorldTrans());
//        m_box.Render();
//    }

    this->renderModel();

    glDepthMask(GL_FALSE);
}

void ModelFaceDeferred::doFinalPass() {
    this->gBuffer.bindForFinalPass();
    int w = Settings::Instance()->SDL_Window_Width;
    int h = Settings::Instance()->SDL_Window_Height;
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
