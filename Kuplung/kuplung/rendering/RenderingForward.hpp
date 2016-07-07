//
//  RenderingForward.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingForward_hpp
#define RenderingForward_hpp

#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/meshes/scene/ModelFaceForward.hpp"

class RenderingForward {
public:
    bool init();
    void render(std::vector<ModelFaceBase*> meshModelFaces,
                glm::mat4 matrixProjection,
                glm::mat4 matrixCamera,
                glm::vec3 vecCameraPosition,
                WorldGrid *grid,
                glm::vec3 uiAmbientLight,
                int lightingPass_DrawMode);

private:
    GLUtils *glUtils;
};

#endif /* RenderingForward_hpp */
