//
//  AssimpParser.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef AssimpParser_hpp
#define AssimpParser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ParserUtils.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpParser {
public:
    ~AssimpParser();
    void init(const std::function<void(float)>& doProgress);
    std::vector<MeshModel> parse(const FBEntity& file, const std::vector<std::string>& settings);

    std::vector<float> vertices, normals;

private:
    std::function<void(float)> funcProgress;

    std::unique_ptr<ParserUtils> parserUtils;

    FBEntity file;
    std::vector<MeshModel> models;
    Assimp::Importer parser;

    int indexModel = -1, indexFace = -1, indexMaterial = -1, indicesCounter = 0, modelID = 1, faceID = 1, meshCounter = 0;
    std::vector<glm::vec3> vectorsVertices, vectorsNormals;
    std::vector<glm::vec2> vectorsTextureCoordinates;
    std::vector<MeshMaterialTextureImage> textures_loaded;

    void processNode(aiNode* node, const aiScene* scene);
    MeshModel processMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelTitle);
    std::vector<MeshMaterialTextureImage> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};

#endif /* AssimpParser_hpp */
