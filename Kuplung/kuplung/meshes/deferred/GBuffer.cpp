//
//  GBuffer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
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

    if (this->outputTexture != 0)
        glDeleteTextures(1, &this->outputTexture);
}

bool GBuffer::init() {
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);

    int windowWidth = Settings::Instance()->SDL_Window_Width;
    int windowHeight = Settings::Instance()->SDL_Window_Height;

    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(this->textures), this->textures);
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(this->textures) ; i++) {
        glBindTexture(GL_TEXTURE_2D, this->textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->textures[i], 0);
    }

    // depth
    glGenTextures(1, &this->depthTexture);
    glBindTexture(GL_TEXTURE_2D, this->depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture, 0);

    // final
    glGenTextures(1, &this->outputTexture);
    glBindTexture(GL_TEXTURE_2D, this->outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, this->outputTexture, 0);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[GBuffer] Framebuffer error, status: 0x%x\n", Status));
        return false;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void GBuffer::startFrame() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GBuffer::bindForGeomPass() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
                             GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
}

void GBuffer::bindForStencilPass() {
    glDrawBuffer(GL_NONE);
}

void GBuffer::bindForLightPass() {
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(this->textures); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, this->textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}

void GBuffer::bindForFinalPass() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
}
