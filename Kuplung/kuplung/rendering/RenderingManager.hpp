//
//  RenderingManager.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RenderingManager_hpp
#define RenderingManager_hpp

#include "kuplung/rendering/methods/RenderingSimple.hpp"
#include "kuplung/rendering/methods/RenderingForward.hpp"
#include "kuplung/rendering/methods/RenderingForwardShadowMapping.hpp"
#include "kuplung/rendering/methods/RenderingDeferred.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/settings/SettingsStructs.h"

class RenderingManager {
public:
    explicit RenderingManager(ObjectsManager &managerObjects);
    ~RenderingManager();
    void init();
    void render(const int& selectedModel);

    int RenderingTotalVertices;
    int RenderingTotalIndices;
    int RenderingTotalTriangles;
    int RenderingTotalFaces;

    std::vector<ModelFaceData*> meshModelFaces;

private:
    ObjectsManager &managerObjects;

    std::unique_ptr<RenderingSimple> rendererSimple;
    std::unique_ptr<RenderingForward> rendererForward;
    std::unique_ptr<RenderingForwardShadowMapping> rendererForwardShadowMapping;
    std::unique_ptr<RenderingDeferred> rendererDeferred;
};

#endif /* RenderingManager_hpp */
