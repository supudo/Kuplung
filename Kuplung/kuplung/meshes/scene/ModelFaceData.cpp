//
//  ModelFaceData.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceData.hpp"
#include <boost/filesystem.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

ModelFaceData::~ModelFaceData() {
    this->destroy();
}

void ModelFaceData::destroy() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboNormals);
    glDeleteBuffers(1, &this->vboTextureCoordinates);
    glDeleteBuffers(1, &this->vboIndices);
    glDeleteBuffers(1, &this->vboTangents);
    glDeleteBuffers(1, &this->vboBitangents);

    glDeleteTextures(1, &this->vboTextureAmbient);
    glDeleteTextures(1, &this->vboTextureDiffuse);
    glDeleteTextures(1, &this->vboTextureSpecular);
    glDeleteTextures(1, &this->vboTextureSpecularExp);
    glDeleteTextures(1, &this->vboTextureDissolve);
    glDeleteTextures(1, &this->vboTextureBump);
    glDeleteTextures(1, &this->vboTextureDisplacement);

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

    glDeleteVertexArrays(1, &this->glVAO);

    ModelFaceBase::~ModelFaceBase();
}

void ModelFaceData::init(MeshModel model, std::string assetsFolder) {
    ModelFaceBase::init(model, assetsFolder);

    this->mathHelper = new Maths();
    this->meshModel = model;
    this->assetsFolder = assetsFolder;

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.vertices.size() * sizeof(glm::vec3), &this->meshModel.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.normals.size() * sizeof(glm::vec3), &this->meshModel.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // textures and colors
    if (this->meshModel.texture_coordinates.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->meshModel.texture_coordinates.size() * sizeof(glm::vec2), &this->meshModel.texture_coordinates[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

        // ambient texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureAmbient, objMaterialImageType_Bump, &this->vboTextureAmbient);

        // diffuse texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDiffuse, objMaterialImageType_Bump, &this->vboTextureDiffuse);

        // specular texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecular, objMaterialImageType_Specular, &this->vboTextureSpecular);

        // specular-exp texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecularExp, objMaterialImageType_SpecularExp, &this->vboTextureSpecularExp);

        // dissolve texture image
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDissolve, objMaterialImageType_Dissolve, &this->vboTextureDissolve);

        // bump map texture
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureBump, objMaterialImageType_Bump, &this->vboTextureBump);

        // displacement map texture
        ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDisplacement, objMaterialImageType_Displacement, &this->vboTextureDisplacement);
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
        glBufferData(GL_ARRAY_BUFFER, int(tangents.size()) * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // bitangents
        glGenBuffers(1, &this->vboBitangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboBitangents);
        glBufferData(GL_ARRAY_BUFFER, int(bitangents.size()) * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    }

    glBindVertexArray(0);
}

void ModelFaceData::renderModel(bool useTessellation) {
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(this->glVAO);

    if (useTessellation)
        glDrawElements(GL_PATCHES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);
    else
        glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (Settings::Instance()->ShowBoundingBox && this->so_selectedYn) {
        glm::mat4 matrixBB = glm::mat4(1.0f);
        matrixBB *= this->matrixProjection;
        matrixBB *= this->matrixCamera;
        matrixBB *= this->matrixModel;
        this->boundingBox->render(matrixBB, this->so_outlineColor);
    }
}
