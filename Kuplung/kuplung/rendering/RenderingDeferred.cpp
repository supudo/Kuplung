//
//  RenderingDeferred.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingDeferred.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

bool RenderingDeferred::init() {
    this->glUtils = new GLUtils();
    this->modelsInitialized = false;

    // Gemetry Pass
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());

    this->shaderProgram_GeometryPass = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram_GeometryPass);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram_GeometryPass, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[Deferred - Geometry Pass] Error linking program " + std::to_string(this->shaderProgram_GeometryPass) + "!");
        this->glUtils->printProgramLog(this->shaderProgram_GeometryPass);
        return false;
    }

    // Lighting Pass
    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.vert";
    shaderSourceVertex = readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.frag";
    shaderSourceFragment = readFile(shaderPath.c_str());

    this->shaderProgram_LightingPass = glCreateProgram();

    shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram_LightingPass);

    programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram_LightingPass, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[Deferred - Lighting Pass] Error linking program " + std::to_string(this->shaderProgram_LightingPass) + "!");
        this->glUtils->printProgramLog(this->shaderProgram_LightingPass);
        return false;
    }

    // Light Boxes
    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_light_box.vert";
    shaderSourceVertex = readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_light_box.frag";
    shaderSourceFragment = readFile(shaderPath.c_str());

    this->shaderProgram_LightBox = glCreateProgram();

    shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightBox, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightBox, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram_LightBox);

    programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram_LightBox, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[Deferred - Light Box] Error linking program " + std::to_string(this->shaderProgram_LightBox) + "!");
        this->glUtils->printProgramLog(this->shaderProgram_LightBox);
        return false;
    }

    // Set samplers
    glUseProgram(this->shaderProgram_LightingPass);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "gAlbedoSpec"), 2);

    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
    this->objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
    this->objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
    this->objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
    this->objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
    this->objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
    this->objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

    // - Colors
    srand(13);
    for (GLuint i = 0; i < this->NR_LIGHTS; i++) {
        // Calculate slightly random offsets
        GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        this->lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

        // Also calculate random color
        GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
        GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
        GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
        this->lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }

    // Set up G-Buffer
    // 3 textures:
    // 1. Positions (RGB)
    // 2. Color (RGB) + Specular (A)
    // 3. Normals (RGB)
    glGenFramebuffers(1, &this->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);

    // - Position color buffer
    glGenTextures(1, &this->gPosition);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gPosition, 0);

    // - Normal color buffer
    glGenTextures(1, &this->gNormal);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gNormal, 0);

    // - Color + Specular color buffer
    glGenTextures(1, &this->gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->gAlbedoSpec, 0);

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // - Create and attach depth buffer (renderbuffer)
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Settings::Instance()->funcDoLog("[Deferred Rendering T GBuffer] Framebuffer not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    return true;
}

void RenderingDeferred::initModels(std::vector<ModelFaceBase*> meshModelFaces) {
    for (size_t j=0; j<meshModelFaces.size(); j++) {
        ModelFaceDeferred *mfdm = new ModelFaceDeferred();
        mfdm->init(meshModelFaces[j]->meshModel, Settings::Instance()->currentFolder);
        this->models.push_back(mfdm);
    }
    this->modelsInitialized = true;
}

void RenderingDeferred::render(std::vector<ModelFaceBase*> meshModelFaces, glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight, int lightingPass_DrawMode) {
    if (!this->modelsInitialized)
        this->initModels(meshModelFaces);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 1. Geometry Pass: render scene's geometry/color data into gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = matrixProjection;
        glm::mat4 view = matrixCamera;
        glm::mat4 model;
        glUseProgram(this->shaderProgram_GeometryPass);
        glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "view"), 1, GL_FALSE, glm::value_ptr(view));
        for (GLuint i = 0; i < this->objectPositions.size(); i++) {
            model = glm::mat4();
            model = glm::translate(model, this->objectPositions[i]);
            model = glm::scale(model, glm::vec3(0.25f));
            glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "model"), 1, GL_FALSE, glm::value_ptr(model));

            for (size_t j=0; j<models.size(); j++) {
                models[j]->renderModel(this->shaderProgram_GeometryPass);
            }
        }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(this->shaderProgram_LightingPass);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

    // Also send light relevant uniforms
    for (GLuint i = 0; i < this->lightPositions.size(); i++) {
        glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &this->lightPositions[i][0]);
        glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &this->lightColors[i][0]);

        // Update attenuation parameters and calculate radius
        const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
        const GLfloat linear = 0.7;
        const GLfloat quadratic = 1.8;
        glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
        glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);

        // Then calculate radius of light volume/sphere
        const GLfloat lightThreshold = 5.0; // 5 / 256
        const GLfloat maxBrightness = std::fmaxf(std::fmaxf(this->lightColors[i].r, this->lightColors[i].g), this->lightColors[i].b);
        GLfloat radius = (-linear + static_cast<float>(std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / lightThreshold) * maxBrightness)))) / (2 * quadratic);
        glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Radius").c_str()), radius);
    }
    glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, "viewPos"), 1, &vecCameraPosition[0]);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "draw_mode"), lightingPass_DrawMode + 1);
    this->renderQuad();

    // 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
    // depth buffer in another stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0,
                      Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
                      0, 0,
                      Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Render lights on top of scene, by blitting
    glUseProgram(this->shaderProgram_LightBox);
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_LightBox, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_LightBox, "view"), 1, GL_FALSE, glm::value_ptr(view));
    for (GLuint i = 0; i < this->lightPositions.size(); i++)
    {
        model = glm::mat4();
        model = glm::translate(model, lightPositions[i]);
        model = glm::scale(model, glm::vec3(0.25f));
        glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_LightBox, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(this->shaderProgram_LightBox, "lightColor"), 1, &lightColors[i][0]);
        this->renderCube();
    }
}

void RenderingDeferred::renderQuad() {
    if (this->quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &this->quadVAO);
        glGenBuffers(1, &this->quadVBO);
        glBindVertexArray(this->quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


void RenderingDeferred::renderCube() {
    // Initialize (if necessary)
    if (this->cubeVAO == 0)
    {
        GLfloat vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
        };
        glGenVertexArrays(1, &this->cubeVAO);
        glGenBuffers(1, &this->cubeVBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, this->cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(this->cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // Render Cube
    glBindVertexArray(this->cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

std::string RenderingDeferred::readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        Settings::Instance()->funcDoLog("[RenderingDeferred] Could not read file " + std::string(filePath) + ". File does not exist.");
        return "";
    }
    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

