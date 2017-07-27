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
#include "kuplung/utilities/imgui/imgui.h"

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

Shadertoy::~Shadertoy() {
    glDeleteBuffers(1, &this->vboVertices);

    glDeleteTextures(1, &this->iChannel0);
    glDeleteTextures(1, &this->iChannel1);
    glDeleteTextures(1, &this->iChannel2);
    glDeleteTextures(1, &this->iChannel3);

    glDisableVertexAttribArray(0);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

Shadertoy::Shadertoy() {
    this->textureWidth = 0;
    this->textureHeight = 0;
    this->iChannel0_Image.clear();
    this->iChannel1_Image.clear();
    this->iChannel2_Image.clear();
    this->iChannel3_Image.clear();
    this->iChannel0_CubeImage.clear();
    this->iChannel1_CubeImage.clear();
    this->iChannel2_CubeImage.clear();
    this->iChannel3_CubeImage.clear();
}

bool Shadertoy::initShaderProgram(std::string const& fragmentShaderSource) {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/shadertoy.vert";
    std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    std::string shaderFragmentSource = "#version 410 core\n\
\n\
out vec4 outFragmentColor;\n\
uniform vec3 iResolution;\n\
uniform float iGlobalTime;\n\
uniform float iTimeDelta;\n\
uniform int iFrame;\n\
uniform int iFrameRate;\n\
uniform float iChannelTime[4];\n\
uniform vec3 iChannelResolution[4];\n\
uniform vec4 iMouse;\n\
uniform vec4 iDate;\n\
uniform float iSampleRate;\n";

    if (!this->iChannel0_Image.empty())
		shaderFragmentSource += "uniform sampler2D iChannel0;\n";
    else if (!this->iChannel0_CubeImage.empty())
		shaderFragmentSource += "uniform samplerCube iChannel0;\n";

    if (!this->iChannel1_Image.empty())
		shaderFragmentSource += "uniform sampler2D iChannel1;\n";
    else if (!this->iChannel1_CubeImage.empty())
		shaderFragmentSource += "uniform samplerCube iChannel1;\n";

    if (!this->iChannel2_Image.empty())
		shaderFragmentSource += "uniform sampler2D iChannel2;\n";
    else if (!this->iChannel2_CubeImage.empty())
		shaderFragmentSource += "uniform samplerCube iChannel2;\n";

    if (!this->iChannel3_Image.empty())
		shaderFragmentSource += "uniform sampler2D iChannel3;\n";
    else if (!this->iChannel3_CubeImage.empty())
		shaderFragmentSource += "uniform samplerCube iChannel3;\n";

	shaderFragmentSource += "\
\n\
#define texture2D texture\n\
#define textureCube texture\n\
\n\
\n";

	shaderFragmentSource += fragmentShaderSource;

	shaderFragmentSource += "\
\n\
void main() {\n\
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n\
    mainImage(color, gl_FragCoord.xy);\n\
    outFragmentColor = color;\n\
}\n\
\n";

    const char *shader_fragment = shaderFragmentSource.c_str();

//    printf("---------------------\n");
//    printf("%s\n", shader_fragment);
//    printf("---------------------\n");

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[ShaderToy] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->vs_InFBO = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "vs_inFBO");
        this->vs_ScreenResolution = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "vs_screenResolution");

        this->iResolution = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iResolution");
        this->iGlobalTime = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iGlobalTime");
        this->iTimeDelta = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iTimeDelta");
        this->iFrameRate = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iFrameRate");
        this->iFrame = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "iFrame");
        this->iChannelTime[0] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelTime[0]");
        this->iChannelTime[1] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelTime[1]");
        this->iChannelTime[2] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelTime[2]");
        this->iChannelTime[3] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelTime[3]");
        this->iChannelResolution[0] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelResolution[0]");
        this->iChannelResolution[1] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelResolution[1]");
        this->iChannelResolution[2] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelResolution[2]");
        this->iChannelResolution[3] = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannelResolution[3]");
        this->iMouse = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iMouse");
        this->iDate = Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iDate");
        this->iChannel0 = static_cast<GLuint>(Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannel0"));
        this->iChannel1 = static_cast<GLuint>(Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannel1"));
        this->iChannel2 = static_cast<GLuint>(Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannel2"));
        this->iChannel3 = static_cast<GLuint>(Settings::Instance()->glUtils->glGetUniformNoWarning(this->shaderProgram, "iChannel3"));
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

    const GLfloat vertices[] = {
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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    this->iChannel0_Image = Settings::Instance()->appFolder() + "/noise16.png";
    this->initTextures();

    glBindVertexArray(0);
}

void Shadertoy::initTextures() {
    int tc = 0;
    if (!this->iChannel0_Image.empty()) {
        this->addTexture(this->iChannel0_Image, &this->iChannel0, tc);
        tc += 1;
    }
    if (!this->iChannel1_Image.empty()) {
        this->addTexture(this->iChannel1_Image, &this->iChannel1, tc);
        tc += 1;
    }
    if (!this->iChannel2_Image.empty()) {
        this->addTexture(this->iChannel2_Image, &this->iChannel2, tc);
        tc += 1;
    }
    if (!this->iChannel3_Image.empty()) {
        this->addTexture(this->iChannel3_Image, &this->iChannel3, tc);
//        tc += 1;
    }
}

void Shadertoy::addTexture(std::string const & textureImage, GLuint* vboTexture, const int textureID) {
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

        GLenum textureFormat = 0;
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
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(textureFormat), tWidth, tHeight, 0, textureFormat, GL_UNSIGNED_BYTE, tPixels);
        stbi_image_free(tPixels);
        switch (textureID) {
            case 0: {
                this->iChannelResolution0[0] = tWidth;
                this->iChannelResolution0[1] = tHeight;
                break;
            }
            case 1: {
                this->iChannelResolution1[0] = tWidth;
                this->iChannelResolution1[1] = tHeight;
                break;
            }
            case 2: {
                this->iChannelResolution2[0] = tWidth;
                this->iChannelResolution2[1] = tHeight;
                break;
            }
            case 3: {
                this->iChannelResolution3[0] = tWidth;
                this->iChannelResolution3[1] = tHeight;
                break;
            }
        }
    }
}

void Shadertoy::render(const int mouseX, const int mouseY, const float seconds) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glEnable(GL_TEXTURE_2D);

        glUniform1i(this->vs_InFBO, 1);

        GLint tc = 0;
        if (!this->iChannel0_Image.empty()) {
            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + tc));
            glBindTexture(GL_TEXTURE_2D, this->iChannel0);
            glUniform1i(static_cast<GLint>(this->iChannel0), tc);
            glUniform3f(this->iChannelResolution[0], this->iChannelResolution0[0], this->iChannelResolution0[1], 0.0f);
            tc += 1;
        }
        if (!this->iChannel1_Image.empty()) {
            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + tc));
            glBindTexture(GL_TEXTURE_2D, this->iChannel1);
            glUniform1i(static_cast<GLint>(this->iChannel0), tc);
            glUniform3f(this->iChannelResolution[0], this->iChannelResolution1[0], this->iChannelResolution1[1], 0.0f);
            tc += 1;
        }
        if (!this->iChannel2_Image.empty()) {
            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + tc));
            glBindTexture(GL_TEXTURE_2D, this->iChannel2);
            glUniform1i(static_cast<GLint>(this->iChannel0), tc);
            glUniform3f(this->iChannelResolution[0], this->iChannelResolution2[0], this->iChannelResolution2[1], 0.0f);
            tc += 1;
        }
        if (!this->iChannel3_Image.empty()) {
            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + tc));
            glBindTexture(GL_TEXTURE_2D, this->iChannel3);
            glUniform1i(static_cast<GLint>(this->iChannel0), tc);
            glUniform3f(this->iChannelResolution[0], this->iChannelResolution3[0], this->iChannelResolution3[1], 0.0f);
        }

        glUniform2f(this->vs_ScreenResolution, this->textureWidth, this->textureHeight);
        glUniform3f(this->iResolution, this->textureWidth, this->textureHeight, 0);
        glUniform1f(this->iGlobalTime, seconds);
        glUniform4f(this->iMouse, static_cast<float>(mouseX), static_cast<float>(mouseY), 0.0f, 0.0f);
        glUniform1f(this->iChannelTime[0], seconds);
        glUniform1f(this->iChannelTime[1], seconds);
        glUniform1f(this->iChannelTime[2], seconds);
        glUniform1f(this->iChannelTime[3], seconds);
        glUniform1f(this->iTimeDelta, ImGui::GetIO().DeltaTime);

        time_t t = time(0);
        const struct tm * now = localtime(&t);
		const float iDate_year = static_cast<float>(now->tm_year + 1900);
		const float iDate_month = static_cast<float>(now->tm_mon + 1);
		const float iDate_day = static_cast<float>(now->tm_mday);
        const float iDate_seconds = static_cast<float>(now->tm_sec);
        glUniform4f(this->iDate, iDate_year, iDate_month, iDate_day, iDate_seconds);

        glUniform1f(this->iFrameRate, ImGui::GetIO().Framerate);
        glUniform1f(this->iFrame, 0.0f);

        // draw
        glBindVertexArray(this->glVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glUseProgram(0);
    }
}

void Shadertoy::initFBO(const int windowWidth, const int windowHeight, GLuint* vboTexture) {
    this->textureWidth = windowWidth;
    this->textureHeight = windowHeight;

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

void Shadertoy::renderToTexture(const int mouseX, const int mouseY, const float seconds, GLuint* vboTexture) {
    this->bindFBO();
    this->render(mouseX, mouseY, seconds);
    this->unbindFBO(vboTexture);
}
