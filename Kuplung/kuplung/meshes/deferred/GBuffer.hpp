//
//  GBuffer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GBuffer_hpp
#define GBuffer_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"

class GBuffer {
public:
    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_TEXCOORD,
        GBUFFER_NUM_TEXTURES
    };

    GBuffer();
    ~GBuffer();
    bool init(int windowWidth, int windowHeight);
    void bindForWriting();
    void bindForReading();
    void setReadBuffer(GBUFFER_TEXTURE_TYPE TextureType);

private:
    GLuint fbo;
    GLuint textures[GBUFFER_NUM_TEXTURES];
    GLuint depthTexture;
    GLuint outputTexture;
};

#endif /* GBuffer_hpp */
