//
//  RayTracerRenderer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RayTracerRenderer.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "kuplung/utilities/stb/stb_image_write.h"

RayTracerRenderer::RayTracerRenderer(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
}

RayTracerRenderer::~RayTracerRenderer() {
    GLint maxColorAttachments = 1;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    GLuint colorAttachment;
    GLenum att = GL_COLOR_ATTACHMENT0;
    for (colorAttachment = 0; colorAttachment < static_cast<GLuint>(maxColorAttachments); colorAttachment++) {
        att += colorAttachment;
        GLint param;
        GLuint objName;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
        if (GL_RENDERBUFFER == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = reinterpret_cast<GLuint*>(&param)[0];
            glDeleteRenderbuffers(1, &objName);
        }
        else if (GL_TEXTURE == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = reinterpret_cast<GLuint*>(&param)[0];
            glDeleteTextures(1, &objName);
        }
    }

    glDeleteFramebuffers(1, &this->renderFBO);
    glDeleteRenderbuffers(1, &this->renderRBO);

    this->glUtils.reset();
}

void RayTracerRenderer::init() {
    this->glUtils = std::make_unique<GLUtils>();
}

std::string RayTracerRenderer::renderImage(FBEntity const& file, std::vector<ModelFaceBase*> *meshModelFaces) {
    std::string endFile;

    int width = Settings::Instance()->SDL_Window_Width;
    int height = Settings::Instance()->SDL_Window_Height;

    this->createFBO();

    glBindFramebuffer(GL_FRAMEBUFFER, this->renderFBO);
    this->renderSceneToFBO(meshModelFaces);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->renderTextureColorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned char* pixels = new unsigned char[3 * width * height];

    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->renderFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned char* line_tmp = new unsigned char[3 * width];
    unsigned char* line_a = pixels;
    unsigned char* line_b = pixels + (3 * width * (height - 1));
    while (line_a < line_b) {
        memcpy(line_tmp, line_a, width * 3);
        memcpy(line_a, line_b, width * 3);
        memcpy(line_b, line_tmp, width * 3);
        line_a += width * 3;
        line_b -= width * 3;
    }

    endFile = file.path + ".bmp";
    stbi_write_bmp(endFile.c_str(), width, height, 3, pixels);

    delete[] pixels;
    delete[] line_tmp;

    return endFile;
}

void RayTracerRenderer::createFBO() {
    glGenFramebuffers(1, &this->renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->renderFBO);
    this->generateAttachmentTexture(false, false);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->renderTextureColorBuffer, 0);

    int screenWidth = Settings::Instance()->SDL_Window_Width;
    int screenHeight = Settings::Instance()->SDL_Window_Height;

    glGenRenderbuffers(1, &this->renderRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->renderRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->renderRBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Settings::Instance()->funcDoLog("[Kuplung-DefaultForwardRenderer] Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RayTracerRenderer::generateAttachmentTexture(GLboolean depth, GLboolean stencil) {
    GLenum attachment_type;
    if (!depth && !stencil)
        attachment_type = GL_RGB;
    else if (depth && !stencil)
        attachment_type = GL_DEPTH_COMPONENT;
    else if (!depth && stencil)
        attachment_type = GL_STENCIL_INDEX;

    int screenWidth = Settings::Instance()->SDL_Window_Width;
    int screenHeight = Settings::Instance()->SDL_Window_Height;

    glGenTextures(1, &this->renderTextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, this->renderTextureColorBuffer);
    if (!depth && !stencil)
        glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, screenWidth, screenHeight, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenWidth, screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RayTracerRenderer::renderSceneToFBO(std::vector<ModelFaceBase*> *meshModelFaces) {
    glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
