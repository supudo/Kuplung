//
//  ModelFaceDeferred.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceDeferred.hpp"
#include <boost/filesystem.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

void ModelFaceDeferred::init(MeshModel model, std::string assetsFolder) {
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
        glBufferData(GL_ARRAY_BUFFER, (int)tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // bitangents
        glGenBuffers(1, &this->vboBitangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboBitangents);
        glBufferData(GL_ARRAY_BUFFER, (int)bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    }

    glBindVertexArray(0);
}

void ModelFaceDeferred::renderModel(GLuint shader) {
    if (this->vboTextureAmbient > 0 && this->meshModel.ModelMaterial.TextureAmbient.UseTexture) {
        glUniform1i(glGetUniformLocation(shader, "sampler_ambient"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureAmbient);
    }

    if (this->vboTextureDiffuse > 0 && this->meshModel.ModelMaterial.TextureDiffuse.UseTexture) {
        glUniform1i(glGetUniformLocation(shader, "sampler_diffuse"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
    }

    if (this->vboTextureSpecular > 0 && this->meshModel.ModelMaterial.TextureSpecular.UseTexture) {
        glUniform1i(glGetUniformLocation(shader, "sampler_specular"), 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecular);
    }

    if (this->vboTextureSpecularExp > 0 && this->meshModel.ModelMaterial.TextureSpecularExp.UseTexture) {
        glUniform1i(glGetUniformLocation(shader, "sampler_specularexp"), 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureSpecularExp);
    }

    if (this->vboTextureDissolve > 0 && this->meshModel.ModelMaterial.TextureDissolve.UseTexture) {
        glUniform1i(glGetUniformLocation(shader, "sampler_dissolve"), 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureDissolve);
    }

    if (this->vboTextureBump > 0 && this->meshModel.ModelMaterial.TextureBump.UseTexture) {
        glUniform1i(glGetUniformLocation(shader, "sampler_bump"), 0);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureBump);
    }

    if (this->vboTextureDisplacement > 0 && this->meshModel.ModelMaterial.TextureDisplacement.UseTexture) {
        glUniform1i(glGetUniformLocation(shader, "sampler_displacement"), 0);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureDisplacement);
    }

    glBindVertexArray(this->glVAO);
    glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
