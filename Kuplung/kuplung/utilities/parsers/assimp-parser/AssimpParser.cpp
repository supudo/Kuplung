//
//  AssimpParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "AssimpParser.hpp"

AssimpParser::~AssimpParser() {
    this->destroy();
}

void AssimpParser::destroy() {
}

void AssimpParser::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
}

objScene AssimpParser::parse(FBEntity file) {
    const aiScene* scene = this->parser.ReadFile(file.path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Settings::Instance()->funcDoLog("[Assimp] Parse error : " + std::string(this->parser.GetErrorString()));
        return this->scene;
    }

    this->indexModel = -1;
    this->indexFace = -1;
    this->indexMaterial = -1;
    this->indicesCounter = 0;
    this->meshCounter = 0;
    this->modelID = 1;
    this->faceID = 1;
    this->vectorsVertices = {};
    this->vectorsTextureCoordinates = {};
    this->vectorsTextureCoordinates = {};
    this->scene = {};
    this->scene.materials = {};
    this->scene.totalCountGeometricVertices = 0;
    this->scene.totalCountTextureCoordinates = 0;
    this->scene.totalCountNormalVertices = 0;
    this->scene.totalCountIndices = 0;
    this->scene.totalCountFaces = 0;
    this->scene.objFile = file.title;

    this->processNode(scene->mRootNode, scene);

    for (size_t i=0; i<this->scene.models.size(); i++) {
        for (size_t j=0; j<this->scene.models[i].faces.size(); j++) {
            this->scene.models[i].faces[j].faceMaterial = this->findMaterial(this->scene.models[i].faces[j].materialID);
        }
    }

    return this->scene;
}

void AssimpParser::processNode(aiNode* node, const aiScene* scene) {
    for (GLuint i=0; i<node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        objModel entityModel;
        entityModel.ID = this->modelID;
        entityModel.modelID = std::string(node->mName.C_Str());
        entityModel.verticesCount = 0;
        entityModel.textureCoordinatesCount = 0;
        entityModel.normalsCount = 0;
        entityModel.indicesCount = 0;
        this->scene.models.push_back(entityModel);
        this->indexModel += 1;
        this->indexFace = -1;
        this->modelID += 1;
        entityModel.faces.push_back(this->processMesh(mesh, scene));
        this->meshCounter += 1;
        this->funcProgress(((float)this->meshCounter / (float)scene->mNumMeshes) * 100.0);
    }
    for (GLuint i=0; i<node->mNumChildren; i++) {
        this->processNode(node->mChildren[i], scene);
    }
}

objModelFace AssimpParser::processMesh(aiMesh* mesh, const aiScene* scene) {
    objModelFace entityFace;
    entityFace.ID = this->faceID;
    entityFace.ModelTitle = this->scene.models[this->indexModel].modelID;
    entityFace.objFile = this->scene.objFile;
    entityFace.ModelID = this->scene.models[this->indexModel].ID;
    entityFace.materialID = "";
    entityFace.verticesCount = 0;
    entityFace.textureCoordinatesCount = 0;
    entityFace.normalsCount = 0;
    entityFace.indicesCount = 0;
    entityFace.solidColor = {};
    this->scene.totalCountFaces += 1;
    this->indexFace += 1;
    this->faceID += 1;

    entityFace.vectors_vertices.insert(
        end(entityFace.vectors_vertices),
        begin(this->vectorsVertices),
        end(this->vectorsVertices)
     );
    entityFace.vectors_texture_coordinates.insert(
        end(entityFace.vectors_texture_coordinates),
        begin(this->vectorsTextureCoordinates),
        end(this->vectorsTextureCoordinates)
     );
    entityFace.vectors_normals.insert(
        end(entityFace.vectors_normals),
        begin(this->vectorsNormals),
        end(this->vectorsNormals)
     );

    // Walk through each of the mesh's vertices
    for (GLuint i=0; i<mesh->mNumVertices; i++) {
        glm::vec3 vector;

        // vertices
        vector = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        entityFace.vertices.push_back(vector.x);
        entityFace.vertices.push_back(vector.y);
        entityFace.vertices.push_back(vector.z);
        entityFace.vectors_vertices.push_back(vector);
        this->scene.totalCountGeometricVertices += 3;
        this->scene.models[this->indexModel].verticesCount += 3;
        entityFace.verticesCount += 3;

        // Normals
        vector = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        entityFace.normals.push_back(vector.x);
        entityFace.normals.push_back(vector.y);
        entityFace.normals.push_back(vector.z);
        entityFace.vectors_normals.push_back(vector);
        this->scene.totalCountNormalVertices += 3;
        this->scene.models[this->indexModel].normalsCount += 3;
        entityFace.normalsCount += 3;

        // Texture coordinates
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            entityFace.texture_coordinates.push_back(vec.x);
            entityFace.texture_coordinates.push_back(vec.y);
            entityFace.vectors_texture_coordinates.push_back(vec);
            this->scene.totalCountTextureCoordinates += 2;
            this->scene.models[this->indexModel].textureCoordinatesCount += 2;
            entityFace.textureCoordinatesCount += 2;
        }
    }

    // Indices
    for (GLuint i=0; i<mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // Retrieve all indices of the face and store them in the indices vector
        for (GLuint j=0; j<face.mNumIndices; j++) {
            entityFace.indices.push_back(face.mIndices[j]);
            this->indicesCounter += 1;
            this->scene.totalCountIndices += 1;
            entityFace.indicesCount += 1;
            this->scene.models[this->indexModel].indicesCount += 1;
        }
    }

    // Materials
    if (mesh->mMaterialIndex > 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiString materialName;
        material->Get(AI_MATKEY_NAME, materialName);
        entityFace.materialID = std::string(materialName.C_Str());

        objMaterial entityMaterial;
        this->indexMaterial += 1;
        entityMaterial.materialID = std::string(materialName.C_Str());
        this->scene.materials.push_back(entityMaterial);

        material->Get(AI_MATKEY_SHININESS_STRENGTH, this->scene.materials[this->indexMaterial].specularExp);
        material->Get(AI_MATKEY_OPACITY, this->scene.materials[this->indexMaterial].transparency);
        // TODO: get illumination model
        this->scene.materials[this->indexMaterial].illumination = 2;
        material->Get(AI_MATKEY_SHININESS, this->scene.materials[this->indexMaterial].opticalDensity);

        aiColor3D color;
        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        this->scene.materials[this->indexMaterial].ambient = { /*.r=*/ color.r, /*.g=*/ color.g, /*.b=*/ color.b, /*.a=*/ 1.0f };
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        this->scene.materials[this->indexMaterial].diffuse = { /*.r=*/ color.r, /*.g=*/ color.g, /*.b=*/ color.b, /*.a=*/ 1.0f };
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        this->scene.materials[this->indexMaterial].specular = { /*.r=*/ color.r, /*.g=*/ color.g, /*.b=*/ color.b, /*.a=*/ 1.0f };
        material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        this->scene.materials[this->indexMaterial].emission = { /*.r=*/ color.r, /*.g=*/ color.g, /*.b=*/ color.b, /*.a=*/ 1.0f };

        // 1. Diffuse maps
        std::vector<objMaterialImage> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        if (diffuseMaps.size() > 0)
            this->scene.materials[this->indexMaterial].textures_diffuse = diffuseMaps[0];

        // 2. Specular maps
        std::vector<objMaterialImage> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        if (specularMaps.size() > 0)
            this->scene.materials[this->indexMaterial].textures_specular = specularMaps[0];

        // 3. Normal maps
        std::vector<objMaterialImage> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        if (normalMaps.size() > 0)
            this->scene.materials[this->indexMaterial].textures_bump = normalMaps[0];

        // 4. Height maps
        std::vector<objMaterialImage> heightMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        if (heightMaps.size() > 0)
            this->scene.materials[this->indexMaterial].textures_displacement = heightMaps[0];

        entityFace.faceMaterial = entityMaterial;
    }

    entityFace.solidColor.push_back(1.0);
    entityFace.solidColor.push_back(0.0);
    entityFace.solidColor.push_back(0.0);
    this->scene.models[this->indexModel].faces.push_back(entityFace);

    return entityFace;
}

std::vector<objMaterialImage> AssimpParser::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<objMaterialImage> textures;
    for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        GLboolean skip = false;
        for (GLuint j = 0; j < textures_loaded.size(); j++) {
            if (this->textures_loaded[j].filename == std::string(str.C_Str())) {
                textures.push_back(this->textures_loaded[j]);
                skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {   // If texture hasn't been loaded already, load it
            objMaterialImage texture;
            texture.filename = std::string(str.C_Str());
            texture.image = std::string(str.C_Str());
            texture.height = 0;
            texture.width = 0;
            texture.useTexture = true;
            texture.commands = {};
            textures.push_back(texture);
            this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}

objMaterial AssimpParser::findMaterial(std::string materialID) {
    for (size_t i=0; i<this->scene.materials.size(); i++) {
        if (materialID == this->scene.materials[i].materialID)
            return this->scene.materials[i];
    }
    return {};
}
