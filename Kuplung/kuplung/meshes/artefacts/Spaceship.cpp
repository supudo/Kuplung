//
//  Spaceship.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Spaceship.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

Spaceship::~Spaceship() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboNormals);
    glDeleteBuffers(1, &this->vboTextureCoordinates);
    glDeleteBuffers(1, &this->vboColors);
    glDeleteBuffers(1, &this->vboIndices);

    glDisableVertexAttribArray(this->glAttributeVertexPosition);
    glDisableVertexAttribArray(this->glAttributeTextureCoord);
    glDisableVertexAttribArray(this->glAttributeVertexNormal);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);

    this->glUtils.reset();
}

#pragma mark - Initialization

Spaceship::Spaceship() {
    this->glUtils = std::make_unique<GLUtils>();
    this->spaceshipGenerator = std::make_unique<SpaceshipMeshGenerator>();

    this->Setting_UseTexture = false;
    this->Setting_Wireframe = false;
}

#pragma mark - Public

bool Spaceship::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/spaceship.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/spaceship.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

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
        Settings::Instance()->funcDoLog("[Spaceship] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
        this->glAttributeVertexNormal = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexNormal");
        this->glAttributeTextureCoord = this->glUtils->glGetAttribute(this->shaderProgram, "a_textureCoord");
        this->glUniformHasTexture = this->glUtils->glGetUniform(this->shaderProgram, "has_texture");
        this->glUniformSamplerTexture = this->glUtils->glGetUniform(this->shaderProgram, "sampler_texture");
        this->glAttributeColor = this->glUtils->glGetAttribute(this->shaderProgram, "a_color");
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glUniformMMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MMatrix");
        this->glFS_CameraPosition = this->glUtils->glGetUniform(this->shaderProgram, "fs_cameraPosition");

        this->solidLight = std::make_unique<ModelFace_LightSource_Directional>();
        this->solidLight->gl_Direction = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.direction");
        this->solidLight->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.ambient");
        this->solidLight->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.diffuse");
        this->solidLight->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.specular");
        this->solidLight->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthAmbient");
        this->solidLight->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthDiffuse");
        this->solidLight->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthSpecular");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void Spaceship::initBuffers(const int gridSize) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    this->spaceshipGenerator->generate(gridSize);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->vertices.size() * sizeof(glm::vec3), &this->spaceshipGenerator->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->normals.size() * sizeof(glm::vec3), &this->spaceshipGenerator->normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexNormal);
    glVertexAttribPointer(this->glAttributeVertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // texture
    if (this->spaceshipGenerator->uvs.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->uvs.size() * sizeof(glm::vec2), &this->spaceshipGenerator->uvs[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeTextureCoord);
        glVertexAttribPointer(this->glAttributeTextureCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
    }

    // colors
    if (this->spaceshipGenerator->colors.size() > 0) {
        glGenBuffers(1, &this->vboColors);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
        glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->colors.size() * sizeof(glm::vec3), &this->spaceshipGenerator->colors[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeColor);
        glVertexAttribPointer(this->glAttributeColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    }

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->spaceshipGenerator->indices.size() * sizeof(GLuint), &this->spaceshipGenerator->indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Render

void Spaceship::render(const glm::mat4 matrixProjection, const glm::mat4 matrixCamera, const glm::mat4 matrixModel, const glm::vec3 vecCameraPosition) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(this->glUniformMMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));

        glUniform1i(this->glUniformHasTexture, this->Setting_UseTexture);
        glUniform1i(this->glUniformSamplerTexture, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);

        glUniform3f(this->glFS_CameraPosition, vecCameraPosition.x, vecCameraPosition.y, vecCameraPosition.z);

        glUniform3f(this->solidLight->gl_Direction, this->lightDirection.x, this->lightDirection.y, this->lightDirection.z);
        glUniform3f(this->solidLight->gl_Ambient, this->solidLightSkin_Ambient.r, this->solidLightSkin_Ambient.g, this->solidLightSkin_Ambient.b);
        glUniform3f(this->solidLight->gl_Diffuse, this->solidLightSkin_Diffuse.r, this->solidLightSkin_Diffuse.g, this->solidLightSkin_Diffuse.b);
        glUniform3f(this->solidLight->gl_Specular, this->solidLightSkin_Specular.r, this->solidLightSkin_Specular.g, this->solidLightSkin_Specular.b);
        glUniform1f(this->solidLight->gl_StrengthAmbient, this->solidLightSkin_Ambient_Strength);
        glUniform1f(this->solidLight->gl_StrengthDiffuse, this->solidLightSkin_Diffuse_Strength);
        glUniform1f(this->solidLight->gl_StrengthSpecular, this->solidLightSkin_Specular_Strength);

        // draw
        glBindVertexArray(this->glVAO);

        if (this->Setting_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawElements(GL_TRIANGLES, int(this->spaceshipGenerator->indices.size()), GL_UNSIGNED_INT, nullptr);

        if (this->Setting_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(0);

        glUseProgram(0);
    }
}
