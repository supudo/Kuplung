//
//  RenderingManager.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingManager_hpp
#define RenderingManager_hpp

#include "kuplung/rendering/RenderingForward.hpp"
#include "kuplung/rendering/RenderingDeferred.hpp"

class RenderingManager {
public:
    void init();
    void render(std::vector<ModelFaceBase*> meshModelFaces,
                glm::mat4 matrixProjection,
                glm::mat4 matrixCamera,
                glm::vec3 vecCameraPosition,
                WorldGrid *grid,
                glm::vec3 uiAmbientLight,
                int lightingPass_DrawMode);

    int RenderingTotalVertices;
    int RenderingTotalIndices;
    int RenderingTotalTriangles;
    int RenderingTotalFaces;

private:
    RenderingForward *renderingForward;
    RenderingDeferred *renderingDeferred;
};

#endif /* RenderingManager_hpp */
