//
//  RenderingManager.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingManager_hpp
#define RenderingManager_hpp

#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/rendering/RenderingSimple.hpp"
#include "kuplung/rendering/RenderingForward.hpp"
#include "kuplung/rendering/RenderingDeferred.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"

class RenderingManager {
public:
    void init();
    void render(std::unique_ptr<ObjectsManager> &managerObjects);

    int RenderingTotalVertices;
    int RenderingTotalIndices;
    int RenderingTotalTriangles;
    int RenderingTotalFaces;

    std::vector<ModelFaceData*> meshModelFaces;

private:
    RenderingSimple *rendererSimple;
    RenderingForward *rendererForward;
    RenderingDeferred *rendererDeferred;
};

#endif /* RenderingManager_hpp */
