//
//  SceneRenderer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SceneRenderer_hpp
#define SceneRenderer_hpp

#include "kuplung/settings/Settings.h"

class SceneRenderer {
public:
    ~SceneRenderer();
    void init();
    void destroy();
    void renderImage(FBEntity file);
};

#endif /* SceneRenderer_hpp */
