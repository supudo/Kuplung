//
//  AssimpParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "AssimpParser.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

AssimpParser::~AssimpParser() {
    this->destroy();
}

void AssimpParser::destroy() {
}

void AssimpParser::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
}

std::vector<MeshModel> AssimpParser::parse(FBEntity file) {
    this->file = file;
    const aiScene* scene = this->parser.ReadFile(file.path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Settings::Instance()->funcDoLog("[Assimp] Parse error : " + std::string(this->parser.GetErrorString()));
        return this->models;
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

    this->processNode(scene->mRootNode, scene);

    return this->models;
}

void AssimpParser::processNode(aiNode* node, const aiScene* scene) {
    for (GLuint i=0; i<node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::string modelTitle = std::string(node->mName.C_Str());
        this->models.push_back(this->processMesh(mesh, scene, modelTitle));
        this->meshCounter += 1;
        this->funcProgress(((float)this->meshCounter / (float)scene->mNumMeshes) * 100.0);
    }
    for (GLuint i=0; i<node->mNumChildren; i++) {
        this->processNode(node->mChildren[i], scene);
    }
}

MeshModel AssimpParser::processMesh(aiMesh* mesh, const aiScene* scene, std::string modelTitle) {
    MeshModel entityModel;
    entityModel.ID = this->indexModel;
    entityModel.ModelTitle = modelTitle;
    entityModel.countVertices = 0;
    entityModel.countTextureCoordinates = 0;
    entityModel.countNormals = 0;
    entityModel.countIndices = 0;

    // Walk through each of the mesh's vertices
    for (GLuint i=0; i<mesh->mNumVertices; i++) {
        // vertices
        entityModel.vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        entityModel.countVertices += 3;

        // Normals
        entityModel.normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        entityModel.countNormals += 3;

        // Texture coordinates
        if (mesh->mTextureCoords[0]) {
            entityModel.texture_coordinates.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
            entityModel.countTextureCoordinates += 2;
        }
    }

    // Indices
    for (GLuint i=0; i<mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (GLuint j=0; j<face.mNumIndices; j++) {
            entityModel.indices.push_back(face.mIndices[j]);
            entityModel.countIndices += 1;
        }
    }

    // Materials
    if (mesh->mMaterialIndex > 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiString materialName;
        material->Get(AI_MATKEY_NAME, materialName);
        entityModel.MaterialTitle = std::string(materialName.C_Str());

        MeshModelMaterial entityMaterial;
        this->indexMaterial += 1;
        entityMaterial.MaterialTitle = std::string(materialName.C_Str());

        float shininess = 0.0f;
        material->Get(AI_MATKEY_SHININESS, shininess); // Ns
        entityMaterial.SpecularExp = shininess / 4.0f; // Assimp still multiples the exp 4 times ....

        material->Get(AI_MATKEY_REFRACTI, entityMaterial.OpticalDensity); // Ni
        material->Get(AI_MATKEY_OPACITY, entityMaterial.Transparency);
        material->Get(AI_MATKEY_SHADING_MODEL, entityMaterial.IlluminationMode);

        aiColor3D color;

        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        entityMaterial.AmbientColor = glm::vec3(color.r, color.g, color.b);

        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        entityMaterial.DiffuseColor = glm::vec3(color.r, color.g, color.b);

        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        entityMaterial.SpecularColor = glm::vec3(color.r, color.g, color.b);

        material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        entityMaterial.EmissionColor = glm::vec3(color.r, color.g, color.b);

        // ambient
        std::vector<MeshMaterialTextureImage> ambientMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT);
        if (ambientMaps.size() > 0)
            entityMaterial.TextureAmbient = ambientMaps[0];

        // diffuse
        std::vector<MeshMaterialTextureImage> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE);
        if (diffuseMaps.size() > 0)
            entityMaterial.TextureDiffuse = diffuseMaps[0];

        // specular
        std::vector<MeshMaterialTextureImage> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR);
        if (specularMaps.size() > 0)
            entityMaterial.TextureSpecular = specularMaps[0];

        // specular exp
        std::vector<MeshMaterialTextureImage> specularExpMaps = this->loadMaterialTextures(material, aiTextureType_SHININESS);
        if (specularExpMaps.size() > 0)
            entityMaterial.TextureSpecularExp = specularExpMaps[0];

        // dissolve
        std::vector<MeshMaterialTextureImage> dissolveMaps = this->loadMaterialTextures(material, aiTextureType_OPACITY);
        if (dissolveMaps.size() > 0)
            entityMaterial.TextureDissolve = dissolveMaps[0];

        // normal/bump
        std::vector<MeshMaterialTextureImage> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT);
        if (normalMaps.size() > 0)
            entityMaterial.TextureBump = normalMaps[0];

        // displacement
        std::vector<MeshMaterialTextureImage> displacementMaps = this->loadMaterialTextures(material, aiTextureType_DISPLACEMENT);
        if (displacementMaps.size() > 0)
            entityMaterial.TextureDisplacement = displacementMaps[0];

        entityModel.ModelMaterial = entityMaterial;
    }

    this->indexModel += 1;

    return entityModel;
}

std::vector<MeshMaterialTextureImage> AssimpParser::loadMaterialTextures(aiMaterial* mat, aiTextureType type) {
    std::vector<MeshMaterialTextureImage> textures;
    for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        GLboolean skip = false;
        for (GLuint j = 0; j < this->textures_loaded.size(); j++) {
            if (this->textures_loaded[j].Filename == std::string(str.C_Str())) {
                textures.push_back(this->textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            MeshMaterialTextureImage texture;
            texture.Filename = std::string(str.C_Str());
            texture.Image = std::string(str.C_Str());

            std::string folderPath = this->file.path;
            boost::replace_all(folderPath, this->file.title, "");
        #ifdef _WIN32
            if (!boost::filesystem::exists(texture.Image))
                texture.Image = folderPath + "/" + texture.Image;
        #else
            if (!boost::filesystem::exists(texture.Image))
                texture.Image = folderPath + "/" + texture.Image;
        #endif

            texture.Height = 0;
            texture.Width = 0;
            texture.UseTexture = true;
            texture.Commands = {};
            textures.push_back(texture);
            this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}
