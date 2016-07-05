//
//  ModelFaceDeferredT.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceDeferredT.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <boost/filesystem.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

void ModelFaceDeferredT::destroy() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboNormals);
    glDeleteBuffers(1, &this->vboTextureCoordinates);
    glDeleteBuffers(1, &this->vboIndices);
    glDeleteBuffers(1, &this->vboTangents);
    glDeleteBuffers(1, &this->vboBitangents);

    GLint maxColorAttachments = 1;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    GLint colorAttachment;
    GLenum att = GL_COLOR_ATTACHMENT0;
    for (colorAttachment = 0; colorAttachment < maxColorAttachments; colorAttachment++) {
        att += colorAttachment;
        GLint param;
        GLuint objName;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
        if (GL_RENDERBUFFER == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteRenderbuffers(1, &objName);
        }
        else if (GL_TEXTURE == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteTextures(1, &objName);
        }
    }

    glDisableVertexAttribArray(this->glVS_VertexPosition);
    glDisableVertexAttribArray(this->glFS_TextureCoord);
    glDisableVertexAttribArray(this->glVS_VertexNormal);
    glDisableVertexAttribArray(this->glVS_Tangent);
    glDisableVertexAttribArray(this->glVS_Bitangent);

    glDeleteProgram(this->shaderProgram);
}

bool ModelFaceDeferredT::initShaderProgram() {
    bool success = true;

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);



    glBindVertexArray(0);

    return success;
}

void ModelFaceDeferredT::initBuffers(std::string assetsFolder) {
    this->assetsFolder = assetsFolder;

    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.vertices.size() * sizeof(glm::vec3), &this->meshModel.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glVS_VertexPosition);
    glVertexAttribPointer(this->glVS_VertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.normals.size() * sizeof(glm::vec3), &this->meshModel.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glVS_VertexNormal);
    glVertexAttribPointer(this->glVS_VertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // textures and colors
    if (this->meshModel.texture_coordinates.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->meshModel.texture_coordinates.size() * sizeof(glm::vec2), &this->meshModel.texture_coordinates[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glFS_TextureCoord);
        glVertexAttribPointer(this->glFS_TextureCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

//        ModelFace::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDiffuse, objMaterialImageType_Diffuse, &this->gl_GeometryPass_TextureDiffuse);
    }

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->meshModel.countIndices * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

    if (this->meshModel.ModelMaterial.TextureBump.Image != "" &&
        this->meshModel.vertices.size() > 0 &&
        this->meshModel.texture_coordinates.size() > 0 &&
        this->meshModel.normals.size() > 0) {
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        this->mathHelper->computeTangentBasis(this->meshModel.vertices, this->meshModel.texture_coordinates, this->meshModel.normals, tangents, bitangents);

        // tangents
        glGenBuffers(1, &this->vboTangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTangents);
        glBufferData(GL_ARRAY_BUFFER, (int)tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glVS_Tangent);
        glVertexAttribPointer(this->glVS_Tangent, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // bitangents
        glGenBuffers(1, &this->vboBitangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboBitangents);
        glBufferData(GL_ARRAY_BUFFER, (int)bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glVS_Bitangent);
        glVertexAttribPointer(this->glVS_Bitangent, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    }

    glBindVertexArray(0);

    this->initBuffersAgain = false;
}

void ModelFaceDeferredT::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
    this->matrixModel = matrixModel;
    this->vecCameraPosition = vecCameraPosition;
    this->grid = grid;
    this->uiAmbientLight = uiAmbientLight;

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
