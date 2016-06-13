//
//  GBuffer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GBuffer_hpp
#define GBuffer_hpp

#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/settings/Settings.h"

class GBuffer {
public:
    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_NUM_TEXTURES
    };
    GBuffer();
    ~GBuffer();

    bool init();
    void startFrame();
    void bindForGeomPass();
    void bindForStencilPass();
    void bindForLightPass();
    void bindForFinalPass();

private:
    GLuint fbo, depthTexture, outputTexture;
    GLuint textures[GBUFFER_NUM_TEXTURES];
};

#endif /* GBuffer_hpp */
