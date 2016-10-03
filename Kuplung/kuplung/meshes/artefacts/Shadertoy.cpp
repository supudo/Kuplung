//
//  Shadertoy.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Shadertoy.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

Shadertoy::~Shadertoy() {
    this->destroy();
}

void Shadertoy::destroy() {
    glDeleteBuffers(1, &this->vboVertices);

    glDeleteTextures(1, &this->iChannel0);
    glDeleteTextures(1, &this->iChannel1);
    glDeleteTextures(1, &this->iChannel2);
    glDeleteTextures(1, &this->iChannel3);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);

    this->glUtils.reset();
}

void Shadertoy::init() {
    this->glUtils = std::make_unique<GLUtils>();
}

bool Shadertoy::initShaderProgram(std::string fragmentShaderSource) {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/shadertoy.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    std::string shaderFragment = "#version 410 core\n\
\n\
out vec4 outFragmentColor;\n\
uniform vec3 iResolution;\n\
uniform float iGlobalTime;\n\
uniform float iTimeDelta;\n\
uniform int iFrame;\n\
uniform float iChannelTime[4];\n\
uniform vec3 iChannelResolution[4];\n\
uniform vec4 iMouse;\n\
uniform vec4 iDate;\n\
uniform float iSampleRate;\n\
uniform sampler2D iChannel0;\n\
uniform sampler2D iChannel1;\n\
uniform sampler2D iChannel2;\n\
uniform sampler2D iChannel3;\n\
\n\
\n";

    shaderFragment += fragmentShaderSource;

    shaderFragment += "\
\n\
void main() {\n\
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n\
    mainImage(color, gl_FragCoord.xy);\n\
    outFragmentColor = color;\n\
}\n\
\n";

    const char *shader_fragment = shaderFragment.c_str();

    printf("---------------------\n");
    printf("%s\n", shader_fragment);
    printf("---------------------\n");

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
        this->vs_InFBO = this->glUtils->glGetUniform(this->shaderProgram, "vs_screenResolution");
        this->vs_ScreenResolution = this->glUtils->glGetUniform(this->shaderProgram, "vs_screenResolution");

        this->iResolution = this->glUtils->glGetUniform(this->shaderProgram, "iResolution");
        this->iGlobalTime = this->glUtils->glGetUniform(this->shaderProgram, "iGlobalTime");
        this->iTimeDelta = this->glUtils->glGetUniform(this->shaderProgram, "iTimeDelta");
        this->iFrame = this->glUtils->glGetUniform(this->shaderProgram, "iFrame");
        this->iChannelTime[0] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelTime[0]");
        this->iChannelTime[1] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelTime[1]");
        this->iChannelTime[2] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelTime[2]");
        this->iChannelTime[3] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelTime[3]");
        this->iChannelResolution[0] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelResolution[0]");
        this->iChannelResolution[1] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelResolution[1]");
        this->iChannelResolution[2] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelResolution[2]");
        this->iChannelResolution[3] = this->glUtils->glGetUniform(this->shaderProgram, "iChannelResolution[3]");
        this->iMouse = this->glUtils->glGetUniform(this->shaderProgram, "iMouse");
        this->iDate = this->glUtils->glGetUniform(this->shaderProgram, "iDate");
        this->iSampleRate = this->glUtils->glGetUniform(this->shaderProgram, "iSampleRate");
        this->iChannel0 = this->glUtils->glGetUniform(this->shaderProgram, "iChannel0");
        this->iChannel1 = this->glUtils->glGetUniform(this->shaderProgram, "iChannel1");
        this->iChannel2 = this->glUtils->glGetUniform(this->shaderProgram, "iChannel2");
        this->iChannel3 = this->glUtils->glGetUniform(this->shaderProgram, "iChannel3");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void Shadertoy::initBuffers() {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    GLfloat vertices[] = {
        -1.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,
        -1.0f, -1.0f,  0.0f
    };

    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    this->iChannel0_Image = Settings::Instance()->appFolder() + "/noise16.png";
    this->initTextures();

    glBindVertexArray(0);
}

void Shadertoy::initTextures() {
    int tc = 0;
    if (this->iChannel0_Image != "") {
        this->addTexture(this->iChannel0_Image, &this->iChannel0);
        tc += 1;
    }
    if (this->iChannel1_Image != "") {
        this->addTexture(this->iChannel1_Image, &this->iChannel1);
        tc += 1;
    }
    if (this->iChannel2_Image != "") {
        this->addTexture(this->iChannel2_Image, &this->iChannel2);
        tc += 1;
    }
    if (this->iChannel3_Image != "") {
        this->addTexture(this->iChannel3_Image, &this->iChannel3);
        tc += 1;
    }
}

void Shadertoy::addTexture(std::string textureImage, GLuint* vboTexture) {
    int tWidth, tHeight, tChannels;
    unsigned char* tPixels = stbi_load(textureImage.c_str(), &tWidth, &tHeight, &tChannels, 0);
    if (tPixels) {
        glGenTextures(1, vboTexture);
        glBindTexture(GL_TEXTURE_2D, *vboTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        GLint textureFormat = 0;
        switch (tChannels) {
            case 1:
                textureFormat = GL_LUMINANCE;
                break;
            case 2:
                textureFormat = GL_LUMINANCE_ALPHA;
                break;
            case 3:
                textureFormat = GL_RGB;
                break;
            case 4:
                textureFormat = GL_RGBA;
                break;
            default:
                textureFormat = GL_RGB;
                break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, tWidth, tHeight, 0, textureFormat, GL_UNSIGNED_BYTE, tPixels);
        stbi_image_free(tPixels);
    }
}

void Shadertoy::render(int mouseX, int mouseY, float seconds) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        glEnable(GL_TEXTURE_2D);

        glUniform1i(this->vs_InFBO, 1);

        int tc = 0;
        if (this->iChannel0_Image != "") {
            glActiveTexture(GL_TEXTURE0 + tc);
            glBindTexture(GL_TEXTURE_2D, this->iChannel0);
            glUniform1i(this->iChannel0, tc);
            tc += 1;
        }
        if (this->iChannel1_Image != "") {
            glActiveTexture(GL_TEXTURE0 + tc);
            glBindTexture(GL_TEXTURE_2D, this->iChannel1);
            glUniform1i(this->iChannel0, tc);
            tc += 1;
        }
        if (this->iChannel2_Image != "") {
            glActiveTexture(GL_TEXTURE0 + tc);
            glBindTexture(GL_TEXTURE_2D, this->iChannel2);
            glUniform1i(this->iChannel0, tc);
            tc += 1;
        }
        if (this->iChannel3_Image != "") {
            glActiveTexture(GL_TEXTURE0 + tc);
            glBindTexture(GL_TEXTURE_2D, this->iChannel3);
            glUniform1i(this->iChannel0, tc);
            tc += 1;
        }

        glUniform2f(this->vs_ScreenResolution, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
        glUniform3f(this->iResolution, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0);
        glUniform1f(this->iGlobalTime, seconds);
        glUniform4f(this->iMouse, float(mouseX), float(mouseY), 0.0f, 0.0f);

        // draw
        glBindVertexArray(this->glVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glUseProgram(0);
    }
}

void Shadertoy::initFBO(int windowWidth, int windowHeight, GLuint* vboTexture) {
    glGenTextures(1, vboTexture);
    glBindTexture(GL_TEXTURE_2D, *vboTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &this->tRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->tRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &this->tFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->tFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *vboTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->tRBO);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        Settings::Instance()->funcDoLog("[Shadertoy] - Error creating FBO! - " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadertoy::bindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->tFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Shadertoy::unbindFBO(GLuint* vboTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, *vboTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Shadertoy::renderToTexture(int mouseX, int mouseY, float seconds, GLuint* vboTexture) {
    this->bindFBO();
    this->render(mouseX, mouseY, seconds);
    this->unbindFBO(vboTexture);
}
