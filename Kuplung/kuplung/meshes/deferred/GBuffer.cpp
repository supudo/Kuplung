//
//  GBuffer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "GBuffer.hpp"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

GBuffer::GBuffer() {
    this->fbo = 0;
    this->depthTexture = 0;
    this->outputTexture = 0;
    ZERO_MEM(this->textures);
}

GBuffer::~GBuffer() {
    if (this->fbo != 0)
        glDeleteFramebuffers(1, &this->fbo);

    if (this->textures[0] != 0)
        glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(this->textures), this->textures);

    if (this->depthTexture != 0)
        glDeleteTextures(1, &this->depthTexture);
}


bool GBuffer::init(int windowWidth, int windowHeight) {
    // Create the FBO
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    // Create the gbuffer textures
    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(this->textures), this->textures);
    glGenTextures(1, &this->depthTexture);

    int w = Settings::Instance()->SDL_Window_Width;
    int h = Settings::Instance()->SDL_Window_Height;

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(this->textures) ; i++) {
        glBindTexture(GL_TEXTURE_2D, this->textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->textures[i], 0);
    }

    // depth
    glBindTexture(GL_TEXTURE_2D, this->depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
                             GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2,
                             GL_COLOR_ATTACHMENT3 };

    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[GBuffer] Framebuffer error, status: 0x%x\n", Status));
        return false;
    }

    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return true;
}

void GBuffer::bindForWriting() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
}

void GBuffer::bindForReading() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
}

void GBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE TextureType) {
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}
