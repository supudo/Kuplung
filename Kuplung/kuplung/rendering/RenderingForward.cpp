//
//  RenderingForward.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingForward.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

bool RenderingForward::init() {
    this->glUtils = new GLUtils();
    return true;
}

void RenderingForward::render(std::vector<ModelFaceBase*> meshModelFaces, glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight, int lightingPass_DrawMode) {
}

